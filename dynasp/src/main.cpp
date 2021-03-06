#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <dynasp/main>
#include <sharp/main>
#include <htd/main.hpp>

#include <memory>
#include <iostream>
#include <fstream>

#include <cstdlib>
#include <cstring>
#include <csignal>

#ifdef HAVE_UNISTD_H 
	#include <unistd.h>
#else
//FIXME: what if we don't have unistd.h for getopt?
#endif

namespace
{
	const int EXIT_ARGUMENT_ERROR = 1;
	const int EXIT_FILEOPEN_ERROR = 2;
	const int EXIT_PARSING_ERROR = 3;

	struct DynAspOptions
	{
		enum DecompositionHeuristic
		{
			MINIMUM_FILL_EDGES = 0,
			MAXIMUM_CARDINALITY_SEARCH
		};

		DynAspOptions(int argc, char *argv[])
		{
			int opt;
			while((opt = getopt(argc, argv, "onvhbds:c:t:l:")) != -1)
				switch(opt)
				{
				case 'o':
					this->tdopt = true;
					break;
				case 'n':
					this->weak = false;
					break;
				case 'l':
					this->children = (unsigned)strtol(optarg, NULL, 10);
					break;
				case 'v':
					this->displayVersion = true;
					break;

				case 'h':
					this->displayHelp = true;
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
					if(this->printBenchmarks)
						this->printMachineReadable = true;
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

			if(optind + 1 == argc)
			{
				this->readFromFile = true;
				this->fileToRead = argv[optind];
			}
			else if(optind != argc) this->error = true;
		}

		bool error = false;
		bool displayVersion = false; // -v, --version
		bool displayHelp = false; // -h, --help
		bool printBenchmarks = false; // -b, --benchmark
		bool printMachineReadable = false; // 2x -b, --benchmark
		bool decompositionOnly = false; // -d, --decomp
		bool customTreeDecomposition = false; // -t, --tree
		DecompositionHeuristic heuristic = MINIMUM_FILL_EDGES;
		bool useSeed = false; // -s, --seed
		bool weak = true; // -s, --seed
		bool tdopt = false; // -s, --seed
		unsigned seed = 0, children = 3;
		bool readFromFile = false;
		char *fileToRead = nullptr;
		bool customConfiguration = false; // -c <config>, --config=<config>
		dynasp::create::ConfigurationType configuration
			= dynasp::create::INCIDENCE_FULLTUPLE;
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
			<< std::endl
			<< "Report dynasp bugs to " << PACKAGE_BUGREPORT << std::endl;
	}

	void
	printVersion()
	{
		std::cout
			<< PACKAGE_STRING << std::endl
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

void printBenchmarks(int signal)
{
	std::cout << std::endl;
	sharp::Benchmark::printBenchmarks(std::cout, false);

	exit(signal);
}

void printBenchmarksMachineReadable(int signal)
{
	std::cout << std::endl;
	sharp::Benchmark::printBenchmarks(std::cout, true);

	exit(signal);
}

int main(int argc, char *argv[])
{
	sharp::Benchmark::registerTimestamp("program start");

	const DynAspOptions opts(argc, argv);

	if(opts.error)
	{
		printHelp(argv[0]);
		exit(EXIT_ARGUMENT_ERROR);
	}

	if(opts.displayHelp)
	{
		printHelp(argv[0]);
		exit(EXIT_SUCCESS);
	}

	if(opts.displayVersion)
	{
		printVersion();
		exit(EXIT_SUCCESS);
	}

	if(opts.printBenchmarks && opts.printMachineReadable)
	{
		signal(SIGABRT, printBenchmarksMachineReadable);
		signal(SIGFPE, printBenchmarksMachineReadable);
		signal(SIGILL, printBenchmarksMachineReadable);
		signal(SIGINT, printBenchmarksMachineReadable);
		signal(SIGSEGV, printBenchmarksMachineReadable);
		signal(SIGTERM, printBenchmarksMachineReadable);
	}
	else if(opts.printBenchmarks)
	{
		signal(SIGABRT, printBenchmarks);
		signal(SIGFPE, printBenchmarks);
		signal(SIGILL, printBenchmarks);
		signal(SIGINT, printBenchmarks);
		signal(SIGSEGV, printBenchmarks);
		signal(SIGTERM, printBenchmarks);
	}

	if(opts.useSeed)
	{
		std::srand(opts.seed);
	}

	if(opts.customTreeDecomposition)
	{
		htd::IOrderingAlgorithm *ct = nullptr;

		switch(opts.heuristic)
		{
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

	if(opts.customConfiguration) dynasp::create::set(opts.configuration);

	std::ifstream inputFileStream;
	std::istream *inputStream = &std::cin;
	if(opts.readFromFile)
	{
		inputStream = &inputFileStream;
		inputFileStream.open(opts.fileToRead, std::ifstream::in);

		if(inputFileStream.fail() || !inputFileStream.is_open())
		{
			std::cout
				<< "Failed to read from file '" << opts.fileToRead << "'."
				<< std::endl;

			exit(EXIT_FILEOPEN_ERROR);
		}
	}

	std::cout << "Parsing..." << std::endl;
	std::unique_ptr<dynasp::IParser> parser(dynasp::Parser::get());
	std::unique_ptr<dynasp::IGroundAspInstance> instance(
		parser->parse(inputStream));
	parser.reset();
	sharp::Benchmark::registerTimestamp("parsing time");

	if(!instance.get())
		exit(EXIT_PARSING_ERROR);

	std::cout << "Initializing solver..." << std::endl;
	std::unique_ptr<htd::ITreeDecompositionAlgorithm> tdAlgorithm(
			htd::TreeDecompositionAlgorithmFactory::instance()
			.getTreeDecompositionAlgorithm());
	dynasp::DynAspAlgorithm algorithm;
	std::unique_ptr<dynasp::IDynAspCountingSolutionExtractor> extractor(
			dynasp::create::countingSolutionExtractor());
	std::unique_ptr<sharp::ITreeSolver> solver(
			sharp::create::treeSolver(*tdAlgorithm, algorithm, *extractor));

	std::cout << "Decomposing..." << std::endl;
	std::unique_ptr<htd::ITreeDecomposition> td(solver->decompose(*instance, opts.weak, opts.children, opts.tdopt));
	std::cout << "TREEWIDTH: " << td->maximumBagSize() - 1 << std::endl;

	if(!opts.decompositionOnly)
	{
		std::cout << "Solving... " << std::flush;
		std::unique_ptr<dynasp::IDynAspCountingSolution> solution(
				static_cast<dynasp::IDynAspCountingSolution *>(
					solver->solve(*instance, *td)));
		std::cout << "done." << std::endl;

		if(solution->optimalWeight() != (size_t)-1)
			std::cout << "OPTIMAL WEIGHT: " << solution->optimalWeight()
				<< std::endl;
		else
			std::cout << "OPTIMAL WEIGHT: N/A" << std::endl;

		std::cout << "SOLUTION COUNT: " << solution->count() << std::endl;
	}

	if(opts.printBenchmarks)
	{
		std::cout << std::endl;
		sharp::Benchmark::printBenchmarks(std::cout, opts.printMachineReadable);
	}

	return EXIT_SUCCESS;
}
