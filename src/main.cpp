#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dynasp/main>
#include <sharp/main>
#include <sharp/ISharpOutput.hpp>
#include <sharp/TextOutput.hpp>
#include <htd/main.hpp>

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdlib>
#include <cstring>
#include <dynasp/CertificateDynAspTuple.hpp>
#include <csignal>

#include <unistd.h>

namespace {
    const int EXIT_ARGUMENT_ERROR = 1;
    const int EXIT_FILEOPEN_ERROR = 2;
    const int EXIT_PARSING_ERROR = 3;

    struct DynAspOptions {
        enum DecompositionHeuristic {
            MINIMUM_FILL_EDGES = 0,
            MAXIMUM_CARDINALITY_SEARCH
        };

        DynAspOptions(int argc, char *argv[]) {
            int opt;
            while ((opt = getopt(argc, argv, "jpornvhbds:c:t:l:")) != -1)
                switch (opt) {
                    case 'j':
                        this->printJSON = true;
                        break;
                    case 'p':
                        this->displayPCS = true;
                        break;
                    case 'o':
                        this->tdopt = true;
                        break;
                    case 'r':
                        this->reductSpeedup = false;
                        break;
                    case 'v':
                        this->displayVersion = true;
                        break;
                    case 'n':
                        this->weak = false;
                        break;
                    case 'l':
                        this->children = (unsigned) strtol(optarg, NULL, 10);
                        break;
                    case 'h':
                        this->displayHelp = true;
                        break;

                    case 'd':
                        this->decompositionOnly = true;
                        break;

                    case 't':
                        this->customTreeDecomposition = true;
                        if (!std::strcmp(optarg, "mf"))
                            this->heuristic = MINIMUM_FILL_EDGES;
                        else if (!std::strcmp(optarg, "mcs"))
                            this->heuristic = MAXIMUM_CARDINALITY_SEARCH;
                        else
                            this->error = true;
                        break;

                    case 'b':
                        if (this->printBenchmarks)
                            this->printMachineReadable = true;
                        this->printBenchmarks = true;
                        break;

                    case 's':
                        this->useSeed = true;
                        this->seed = (unsigned) strtol(optarg, NULL, 10);
                        break;

                    case 'c':
                        this->customConfiguration = true;
                        switch (strtol(optarg, NULL, 10)) {
                            case 1:
                                this->configuration = dynasp::create::PRIMAL_FULLTUPLE;
                                break;

                            case 2:
                                this->configuration =
                                        dynasp::create::PRIMAL_SIMPLETUPLE;
                                break;

                            case 3:
                                this->configuration =
                                        dynasp::create::PRIMAL_INVERSESIMPLETUPLE;
                                break;

                            case 4:
                                this->configuration =
                                        dynasp::create::INCIDENCE_FULLTUPLE;
                                break;

                            case 5:
                                this->configuration =
                                        dynasp::create::INCIDENCEPRIMAL_FULLTUPLE;
                                break;

                            case 6:
                                this->configuration =
                                        dynasp::create::INCIDENCEPRIMAL_RULESETTUPLE;
                                break;

                            default:
                                this->error = true;
                                break;
                        }
                        break;

                    default:
                        this->error = true;
                        break;
                }

            if (optind + 1 == argc) {
                this->readFromFile = true;
                this->fileToRead = argv[optind];
            }
            else if (optind != argc) this->error = true;
        }

        bool error = false;
        bool displayVersion = false; // -v, --version
        bool reductSpeedup = true; // -v, --version
        bool tdopt = false; // -v, --version
        bool displayHelp = false; // -h, --help
        bool displayPCS = false; // -p, --print-pcs
        bool printBenchmarks = false; // -b, --benchmark
        bool printJSON = false; // -j, --print-json
        bool printMachineReadable = false; // 2x -b, --benchmark
        bool decompositionOnly = false; // -d, --decomp
        bool customTreeDecomposition = false; // -t, --tree
        DecompositionHeuristic heuristic = MINIMUM_FILL_EDGES;
        bool useSeed = false; // -s, --seed
        unsigned seed = 0, children = 3;
        bool weak = true;
        bool readFromFile = false;
        char *fileToRead = nullptr;
        bool customConfiguration = false; // -c <config>, --config=<config>
        dynasp::create::ConfigurationType configuration
                = dynasp::create::INCIDENCE_FULLTUPLE;
    };

    void
    printHelp(const char *programName) {
        std::cout
                << "Usage: " << programName << " [OPTION]... [FILE]"
                << std::endl
                << "Evaluate answer set programs via tree decompositions."
                << std::endl
                << std::endl
                << "Arguments to options are always mandatory. Valid options:"
                << std::endl
                << "  -v\t  output version information and exit" << std::endl
                << "  -h\t  display this help message and exit" << std::endl
                << "  -d\t  perform decomposition and print treewidth" << std::endl
                << "  -t ALG  use ALG for decomposition, where ALG is one of:"
                << std::endl
                << "\t    mcs: maximum cardinality search bucket elimination"
                << std::endl
                << "\t    mf:  minimum fill edge count bucket elimination (default)"
                << std::endl
                << "  -b\t  display timing information (use twice for CSV format)"
                << std::endl
                << "  -s NUM  set NUM as seed for the random number generator"
                << std::endl
                << "  -c NUM  set algorithm configuration to NUM:" << std::endl
                << "\t    1: primal graph, full certificates" << std::endl
                << "\t    2: primal graph, optimized certificates" << std::endl
                << "\t    3: primal graph, inverse certificates" << std::endl
                << "\t    4: incidence graph, full certificates (default)"
                << std::endl
                << "\t    5: incidence graph/primal constraints, full certificates"
                << std::endl
                << "\t    6: incidence graph/primal constraints, optimized "
                << "certificates" << std::endl
                << std::endl
                << "Exit status: " << std::endl
                << " " << EXIT_SUCCESS << "  if OK," << std::endl
                << " " << EXIT_ARGUMENT_ERROR << "  if problems parsing options"
                << std::endl
                << " " << EXIT_FILEOPEN_ERROR << "  if problems opening input file"
                << std::endl
                << " " << EXIT_PARSING_ERROR << "  if problems parsing input"
                << std::endl
                << std::endl;
//        TODO:
//                << "Report dynasp bugs to " << PACKAGE_BUGREPORT << std::endl;
    }

    const char *portfolio_g = {
            "t [mcs,mf] #\0"
                    "c [1,2,3,4,5,6][4]\n"
                    "# algorithm configuration \n"
                    "#   1: primal graph, full certificates;\n"
                    "#   2: primal graph, optimized certificates;\n"
                    "#   3: primal graph, inverse certificates;\n"
                    "#   4: incidence graph, full certificates (default); \n"
                    "#   5: incidence graph/primal constraints, full certificates;\n"
                    "#   6: incidence graph/primal constraints, optimized certificates\0"
                    "s [0,9223372036854775807][0]i #seed for the random number generator"
    };

    void
    printPCS(const char *programName) {
        std::cout << "# generated by '" << programName << " -p'" << std::endl;
//        TODO:
//                  << "# version " << PACKAGE_STRING << std::endl;
        for (const char *p = portfolio_g; *p; p += strlen(p) + 1) {
            std::cout << p << std::endl;
        }
    }

    void
    printVersion() {
        std::cout
//                << PACKAGE_STRING << std::endl
                << "Copyright (C) 2016 Michael Morak" << std::endl
                << "License GPLv3+: GNU GPL version 3 or later "
                << "<http://gnu.org/licenses/gpl.html>." << std::endl
                << "This is free software: you are free to change and "
                << "redistribute it." << std::endl
                << "There is NO WARRANTY, to the extent permitted by law."
                << std::endl
                << std::endl
                << "Written by Michael Morak." << std::endl;
    }
}

/*void sighandler(int)
{
	sharp::Benchmark::interrupt();
}*/

void printBenchmarks(int signal) {
//    TODO: remove parameter
    sharp::Benchmark::printBenchmarks(std::cout, false);
    _exit(signal);
}

int main(int argc, char *argv[]) {
    std::shared_ptr <ISharpOutput> output = std::make_shared<TextOutput>();
    sharp::Benchmark::registerOutput(output);
    try {

        sharp::Benchmark::registerTimestamp("program start");

        const DynAspOptions opts(argc, argv);

        if (opts.error) {
            printHelp(argv[0]);
            exit(EXIT_ARGUMENT_ERROR);
        }

        if (opts.displayHelp) {
            printHelp(argv[0]);
            exit(EXIT_SUCCESS);
        }

        if (opts.displayPCS) {
            printPCS(argv[0]);
            exit(EXIT_SUCCESS);
        }

        if (opts.displayVersion) {
            printVersion();
            exit(EXIT_SUCCESS);
        }

        if (opts.printBenchmarks) {
            signal(SIGINT, printBenchmarks);
        }

        if (opts.useSeed) {
            std::srand(opts.seed);
        }

        if (opts.customTreeDecomposition) {
            htd::IOrderingAlgorithm *ct = nullptr;

            switch (opts.heuristic) {
                case DynAspOptions::MAXIMUM_CARDINALITY_SEARCH:
                    ct = new htd::MaximumCardinalitySearchOrderingAlgorithm();
                    break;

                case DynAspOptions::MINIMUM_FILL_EDGES:
                default:
                    ct = new htd::MinFillOrderingAlgorithm();
                    break;
            }

            htd::OrderingAlgorithmFactory::instance().setConstructionTemplate(ct);
        }

        if (opts.customConfiguration) dynasp::create::set(opts.configuration);

        std::ifstream inputFileStream;
        std::istream *inputStream = &std::cin;
        if (opts.readFromFile) {
            inputStream = &inputFileStream;
            inputFileStream.open(opts.fileToRead, std::ifstream::in);

            if (inputFileStream.fail() || !inputFileStream.is_open()) {
                std::ostringstream err;
                err << "Failed to read from file '" << opts.fileToRead << "'.";
                output->error(err.str());
                exit(EXIT_FILEOPEN_ERROR);
            }
        }

        output->info("Parsing...");
        std::unique_ptr <dynasp::IParser> parser(dynasp::Parser::get());
        std::unique_ptr <dynasp::IGroundAspInstance> instance(
                parser->parse(inputStream));
        parser.reset();
        sharp::Benchmark::registerTimestamp("parsing time");

        if (!instance.get())
            exit(EXIT_PARSING_ERROR);

        output->info("Initializing solver...");
        std::unique_ptr <htd::ITreeDecompositionAlgorithm> tdAlgorithm(
                htd::TreeDecompositionAlgorithmFactory::instance()
                        .getTreeDecompositionAlgorithm());
        dynasp::DynAspAlgorithm algorithm;
        dynasp::DynAspCertificateAlgorithm second_layer_algorithm;
        std::unique_ptr <dynasp::IDynAspCountingSolutionExtractor> extractor(
                dynasp::create::countingSolutionExtractor());
        std::unique_ptr <sharp::ITreeTupleAlgorithm> two_layered_algorithm(
                sharp::create::treeTupleAlgorithm(algorithm, second_layer_algorithm));

        instance->setSpeedup(opts.reductSpeedup);
//#define SINGLE_LAYER

#ifdef SINGLE_LAYER
        std::unique_ptr<sharp::ITreeSolver> solver(
                sharp::create::treeSolver(*tdAlgorithm, algorithm, *extractor));
#else
#ifdef SEVERAL_PASSES
        sharp::TreeTupleAlgorithmVector algs;
        algs.push_back(&algorithm);

#ifdef THREE_PASSES
            dynasp::DynAspAlgorithm algorithm2;
            algorithm2.setFurther(true);

#ifndef MERGE_PASS_TWO_THREE
                algs.push_back(&algorithm2);
#endif
#endif

#ifdef MERGE_PASS_TWO_THREE
            std::unique_ptr<sharp::ITreeTupleAlgorithm> two_layered_algorithm2(
            sharp::create::treeTupleAlgorithm(algorithm2, second_layer_algorithm));

            algs.push_back(*two_layered_algorithm2);
#else
            algs.push_back(&second_layer_algorithm);
#endif
        std::unique_ptr<sharp::ITreeSolver> solver(
                sharp::create::treeSolver(*tdAlgorithm, algs, *extractor));
#else
        std::unique_ptr < sharp::ITreeSolver > solver(
                sharp::create::treeSolver(*tdAlgorithm, *two_layered_algorithm, *extractor));
#endif
#endif

        output->info("Decomposing...");
        std::unique_ptr <htd::ITreeDecomposition> td(
                solver->decompose(*instance, opts.weak, opts.children, opts.tdopt));
        output->data("treewidth", td->maximumBagSize() - 1);

        if (!opts.decompositionOnly) {
            output->info("Solving... ");
            std::unique_ptr <dynasp::IDynAspCountingSolution> solution(
                    static_cast<dynasp::IDynAspCountingSolution *>(
                            solver->solve(*instance, *td)));
            output->info("done.");

            if (solution->optimalWeight() != (size_t) - 1)
                output->data("optimal weight", solution->optimalWeight());
            else
                output->data("optimal weight", "N/A");
            output->data("solution count", solution->count());

        }

        if (opts.printBenchmarks) {
            sharp::Benchmark::printBenchmarks(std::cout, opts.printMachineReadable);
        }

        return EXIT_SUCCESS;
    }
    catch (char const *e) {
        std::cerr << std::endl << "========== UNHANDLED EXCEPTION. ==========" << std::endl <<
                  "Exception text: '" << e << "'" << std::endl << "Exiting ..." << std::endl;
    }
}
