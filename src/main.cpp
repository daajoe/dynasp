#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <dynasp/main>
#include <sharp/main>
#include <sharp/ISharpOutput.hpp>
#include <sharp/CsvOutput.hpp>
#include <sharp/JsonOutput.hpp>
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

DYNASP_HELPER_EXPORT htd::LibraryInstance* inst = NULL;
//htd::createManagementInstance(0);

namespace
{
	const int EXIT_ARGUMENT_ERROR = 1;
	const int EXIT_FILEOPEN_ERROR = 2;
	const int EXIT_PARSING_ERROR = 3;

    struct DynAspOptions {
        enum DecompositionHeuristic {
            MINIMUM_FILL_EDGES = 0,
            MAXIMUM_CARDINALITY_SEARCH
        };

		DynAspOptions(int argc, char *argv[])
		{
			int opt;
			while((opt = getopt(argc, argv, "mp:ornvhbds:c:t:l:")) != -1)
				switch(opt)
				{
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
					this->children = 2;
					break;
				case 'p':
					/*if (this->compr)
						this->passes = 3;
					else*/
					this->passes = (unsigned)strtol(optarg, NULL, 10);
					if (this->passes < 3)
						this->compr = false;
					break;
				case 'm':
					this->compr = false;
					//this->passes = 3;
					break;
				case 'l':
					if (!this->weak)
						this->children = 2;
					else
						this->children = (unsigned)strtol(optarg, NULL, 10);
					break;
				case 'h':
					this->displayHelp = true;
					break;

#TODO: duplicate parameter
                case 'j':
                    this->printJSON = true;
                    break;
                case 'p':
                    this->displayPCS = true;
                    break;

				case 'd':
					this->decompositionOnly = true;
					break;

				case 't':
					this->customTreeDecomposition = true;
					if(!std::strcmp(optarg, "mf"))
						this->heuristic = MINIMUM_FILL_EDGES;
					else if(!std::strcmp(optarg, "mcs"))
						this->heuristic = MAXIMUM_CARDINALITY_SEARCH;
					else
						this->error = true;
					break;

                    case 'b':
                        if (this->printBenchmarks)
                            this->printCSV = true;
                        this->printBenchmarks = true;
                        break;

				case 's':
					this->useSeed = true;
					this->seed = (unsigned)strtol(optarg, NULL, 10);
					break;

				case 'c':
					this->customConfiguration = true;
					switch(strtol(optarg, NULL, 10))
					{
					/*case 1:
						this->configuration = dynasp::create::PRIMAL_FULLTUPLE;
						//break;*/

					case 2:
						this->configuration =
							dynasp::create::PRIMAL_SIMPLETUPLE;
						break;

					/*case 3:
						this->configuration =
							dynasp::create::PRIMAL_INVERSESIMPLETUPLE;
						//break;

					case 4:
						this->configuration =
							dynasp::create::INCIDENCE_FULLTUPLE;
						//break;

					case 5:
						this->configuration =
							dynasp::create::INCIDENCEPRIMAL_FULLTUPLE;
						//break;*/

					case 6:
						this->configuration =
							dynasp::create::INCIDENCEPRIMAL_RULESETTUPLE;

						this->reductSpeedup = false;
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

			if(optind + 1 == argc)
			{
				this->readFromFile = true;
				this->fileToRead = argv[optind];
			}
			else if(optind != argc) this->error = true;
		}

		bool error = false;
		bool displayVersion = false; // -v, --version
		bool reductSpeedup = true; // -v, --version
		bool tdopt = false; // -v, --version
		bool displayHelp = false; // -h, --help
        bool displayPCS = false; // -p, --print-pcs
        bool printBenchmarks = false; // -b, --benchmark
        bool printJSON = false; // -j, --print-json
        bool printCSV = false; // 2x -b, --benchmark
		bool decompositionOnly = false; // -d, --decomp
		bool customTreeDecomposition = false; // -t, --tree
		DecompositionHeuristic heuristic = MINIMUM_FILL_EDGES;
		bool useSeed = false; // -s, --seed
		unsigned seed = 0, children = 3, passes = 3;
		bool weak = true, compr = true;
		bool readFromFile = false;
		char *fileToRead = nullptr;
		bool customConfiguration = false; // -c <config>, --config=<config>
		dynasp::create::ConfigurationType configuration = dynasp::create::PRIMAL_SIMPLETUPLE;
	};

	void
	printHelp(const char *programName)
	{
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
			//<< "\t    1: primal graph, full certificates" << std::endl
			<< "\t    2: primal graph, optimized certificates (default)" << std::endl
			//<< "\t    3: primal graph, inverse certificates" << std::endl
			//<< "\t    4: incidence graph, full certificates"
			//	<< std::endl
			//<< "\t    5: incidence graph/primal constraints, full certificates"
			//	<< std::endl
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
			<< std::endl
			<< "Report dynasp bugs to " << PACKAGE_BUGREPORT << std::endl;
	}

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
	printVersion()
	{
		std::cout
			<< PACKAGE_STRING << std::endl
			<< "Copyright (C) 2016 Michael Morak & Markus Hecher & JKF" << std::endl
			<< "License GPLv3+: GNU GPL version 3 or later "
				<< "<http://gnu.org/licenses/gpl.html>." << std::endl
			<< "This is free software: you are free to change and "
				<< "redistribute it." << std::endl
			<< "There is NO WARRANTY, to the extent permitted by law."
				<< std::endl
			<< std::endl
			<< "Written by Michael Morak & Markus Hecher & JKF." << std::endl;
	}
}

void shutdown(int signal) {
    sharp::Benchmark::printBenchmarks();
    Benchmark::output()->exit();
    _exit(signal);
}

int main(int argc, char *argv[]) {
    inst = htd::createManagementInstance(0);

    const DynAspOptions opts(argc, argv);
    try {
        std::shared_ptr <ISharpOutput> output;
        if (opts.printJSON){
            output = std::make_shared<JsonOutput>();
        }
        else if(opts.printCSV){
            output = std::make_shared<CsvOutput>();
        }
        else{
            output = std::make_shared<TextOutput>();
        }


        sharp::Benchmark::registerOutput(output);
        sharp::Benchmark::registerTimestamp("program start");


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
            signal(SIGINT, shutdown);
        }

        if (opts.useSeed) {
            std::srand(opts.seed);
        }

        if (opts.customTreeDecomposition) {
            htd::IOrderingAlgorithm *ct = nullptr;

		switch(opts.heuristic)
		{
		case DynAspOptions::MAXIMUM_CARDINALITY_SEARCH:
			ct = new htd::MaximumCardinalitySearchOrderingAlgorithm(inst);
			break;

		case DynAspOptions::MINIMUM_FILL_EDGES:
		default:
			ct = new htd::MinFillOrderingAlgorithm(inst);
			break;
		}

		htd::OrderingAlgorithmFactory of(inst);
		of.setConstructionTemplate(ct);
		//htd::OrderingAlgorithmFactory::instance().setConstructionTemplate(ct);
	}

        if (opts.customConfiguration) dynasp::create::set(opts.configuration);

	dynasp::create::setNon(!opts.weak);
	dynasp::create::setReductSpeedup(opts.reductSpeedup);
	dynasp::create::setPasses(opts.passes);
	dynasp::create::setCompr(opts.compr);

	std::ifstream inputFileStream;
	std::istream *inputStream = &std::cin;
	if(opts.readFromFile)
	{
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
//  TODO:
//        ["Equivalences", 1],
//        ["Atom-Atom_Equivalences", 0],
//        ["Body-Body_Equivalences", 0],
//        ["Other_Equivalences", 1],
//        ["Frac_Atom-Atom_Equivalences", 0.0000],
//        ["Frac_Body-Body_Equivalences", 0.0000],
//        ["Frac_Other_Equivalences", 1.0000],
//        ["Created_Bodies", 1],

// TODO: ??
//        ["Assigned_Problem_Variables", 1],
//        ["Nodes_in_Positive_BADG", 0],

// TODO: simple todos
//        ["Frac_Neg_Body", 1.0000],
//        ["Frac_Pos_Body", 1.0000],

//TODO:
//        ["Problem_Variables", 2],
//        ["Free_Problem_Variables", 1],
//        ["SCCS", 0],
//        ["Tight", 1],

        output->data("Static","Program_Atoms", instance.get()->getNumAtoms());
        output->data("Static","Rules", instance.get()->getNumRules());
//        ["Frac_Unary_Rules", 0.5000],
//        ["Frac_Binary_Rules", 0.5000],
//        ["Frac_Ternary_Rules", 0.0000],

        output->data("Static","Constraints", instance.get()->getNumConstraints());
        output->data("Static","Binary_Constraints", instance.get()->getNumBinaryConstraints());
        output->data("Static","Ternary_Constraints", instance.get()->getNumTernaryConstraints());
        output->data("Static","Other_Constraints", instance.get()->getNumOtherConstraints());

        output->data("Static","Cardinality_Rules", instance.get()->getNumCardinalityRules());
        output->data("Static","Choice_Rules", instance.get()->getNumChoiceRules());
        output->data("Static","Disjunctive_Rules", instance.get()->getNumDisjunctiveRules());
        output->data("Static","Normal_Rules", instance.get()->getNumNormalRules());
        output->data("Static","Weight_Rules", instance.get()->getNumWeightedRules());

        //TODO: ??
        output->data("Static","Constraints/Vars", (float) instance.get()->getNumConstraints()/ (float) instance.get()->getNumRules());

        output->data("Static","Frac_Integrity_Rules", (float) instance.get()->getNumConstraints()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Binary_Constraints", (float) instance.get()->getNumBinaryConstraints()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Ternary_Constraints", (float) instance.get()->getNumTernaryConstraints()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Other_Constraints", (float) instance.get()->getNumOtherConstraints()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Cardinality_Rules", (float) instance.get()->getNumCardinalityRules()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Choice_Rules", (float) instance.get()->getNumChoiceRules()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Disjunctive_Rules", (float) instance.get()->getNumDisjunctiveRules()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Normal_Rules", (float) instance.get()->getNumNormalRules()/ (float) instance.get()->getNumRules());
        output->data("Static","Frac_Weight_Rules", (float) instance.get()->getNumWeightedRules()/ (float) instance.get()->getNumRules());

        output->data("Static","Test", (size_t) 2);


	std::cout << "Initializing solver..." << std::endl;

/*	std::unique_ptr<htd::ITreeDecompositionAlgorithm> tdAlgorithm(
			htd::TreeDecompositionAlgorithmFactory::instance()
			.getTreeDecompositionAlgorithm());*/
	std::unique_ptr<htd::ITreeDecompositionAlgorithm> tdAlgorithm(
			htd::TreeDecompositionAlgorithmFactory(inst)
			.getTreeDecompositionAlgorithm());

	dynasp::DynAspAlgorithm algorithm;
	dynasp::DynAspCertificateAlgorithm second_layer_algorithm;
	std::unique_ptr<dynasp::IDynAspCountingSolutionExtractor> extractor(
			dynasp::create::countingSolutionExtractor());
	std::unique_ptr<sharp::ITreeTupleAlgorithm> two_layered_algorithm(
			sharp::create::treeTupleAlgorithm(algorithm, second_layer_algorithm));

	instance->setSpeedup(opts.reductSpeedup);
//#define SINGLE_LAYER

#ifdef SINGLE_LAYER
	std::unique_ptr<sharp::ITreeSolver> solver(
			sharp::create::treeSolver(*tdAlgorithm, algorithm, *extractor));
#else

	#ifdef MERGE_PASS_TWO_THREE
		std::unique_ptr<sharp::ITreeTupleAlgorithm> two_layered_algorithm2 = nullptr;
	#endif
	std::unique_ptr<sharp::ITreeSolver> solver = nullptr;
	sharp::TreeTupleAlgorithmVector algs;
	dynasp::DynAspAlgorithm algorithm2;

	if (opts.passes >= 2) { //#ifdef SEVERAL_PASSES
		//sharp::TreeTupleAlgorithmVector algs;
		algs.push_back(&algorithm);

		if (opts.passes >= 3) { //#ifdef THREE_PASSES
			//dynasp::DynAspAlgorithm algorithm2;
			algorithm2.setFurther(true);

			#ifndef MERGE_PASS_TWO_THREE
				algs.push_back(&algorithm2);
			#endif
		} //#endif

		#ifdef MERGE_PASS_TWO_THREE
			/*std::unique_ptr<sharp::ITreeTupleAlgorithm>*/ two_layered_algorithm2.reset(
			sharp::create::treeTupleAlgorithm(algorithm2, second_layer_algorithm));

			algs.push_back(*two_layered_algorithm2);
		#else
			algs.push_back(&second_layer_algorithm);
		#endif
		/*std::unique_ptr<sharp::ITreeSolver>*/ solver.reset(
				sharp::create::treeSolver(*tdAlgorithm, algs, *extractor));
	} else { //#else
		/*std::unique_ptr<sharp::ITreeSolver>*/ solver.reset(
				sharp::create::treeSolver(*tdAlgorithm, *two_layered_algorithm, *extractor));
	} //#endif
#endif

        output->info("Decomposing...");
        std::unique_ptr <htd::ITreeDecomposition> td(
                solver->decompose(*instance, opts.weak, opts.children, opts.tdopt));
        output->preproc_data("treewidth", td->maximumBagSize() - 1);

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
            sharp::Benchmark::printBenchmarks();
        }

        Benchmark::output()->exit();
        return EXIT_SUCCESS;
    }
    catch (char const *e) {
        std::cerr << std::endl << "========== UNHANDLED EXCEPTION. ==========" << std::endl <<
                  "Exception text: '" << e << "'" << std::endl << "Exiting ..." << std::endl;
    }
}
