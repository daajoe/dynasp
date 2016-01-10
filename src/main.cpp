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
		DynAspOptions(int argc, char *argv[])
		{
			int opt;
			while((opt = getopt(argc, argv, "vhbc:")) != -1)
				switch(opt)
				{
				case 'v':
					this->displayVersion = true;
					break;

				case 'h':
					this->displayHelp = true;
					break;

				case 'b':
					this->printBenchmarks = true;
					break;

				case 'c':
					this->customConfiguration = true;
					switch(strtol(optarg, NULL, 10))
					{
					case 1:
						//TODO
						break;

					case 2:
						//TODO
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
		bool readFromFile = false;
		char *fileToRead = nullptr;
		bool customConfiguration = false; // -c <config>, --config=<config>
		dynasp::create::ConfigurationType configuration
			= dynasp::create::DEFAULT;
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
			<< "Arguments to options are always mandatory." << std::endl
			<< "  -v\t  output version information and exit" << std::endl
			<< "  -h\t  display this help message and exit" << std::endl
			<< "  -b\t  display timing information for benchmarks" << std::endl
			<< "  -c NUM  set algorithm configuration to NUM:" << std::endl
			<< "\t    1: primal graph, full certificates (default)" << std::endl
			<< "\t    2: primal graph, optimized certificates" << std::endl
			<< "\t    3: primal graph, inverse certificates" << std::endl
			<< "\t    4: incidence graph, full certificates" << std::endl
			<< "\t    5: incidence graph/primal constraints, full certificates"
				<< std::endl
			<< "\t    6: incidence graph/primal constraints, optimized "
				<< "certificates" << std::endl
			<< "\t    7: incidence graph/primal constraints, inverse "
				<< "certificates" << std::endl;
	}

	void
	printVersion(const char *programName)
	{
		std::cout
			<< programName << " " << VERSION << std::endl
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

int main(int argc, char *argv[])
{
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
		printVersion(argv[0]);
		exit(EXIT_SUCCESS);
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

	if(!instance.get())
		exit(EXIT_PARSING_ERROR);

	std::cout << "Initializiing solver..." << std::endl;
	std::unique_ptr<htd::ITreeDecompositionAlgorithm> tdAlgorithm(
			htd::TreeDecompositionAlgorithmFactory::instance()
			.getTreeDecompositionAlgorithm());
	dynasp::DynAspAlgorithm algorithm;
	std::unique_ptr<dynasp::IDynAspCountingSolutionExtractor> extractor(
			dynasp::create::countingSolutionExtractor());
	sharp::IterativeTreeTupleSolver s(*tdAlgorithm, algorithm, *extractor); 

	std::cout << "Solving..." << std::flush;
	std::unique_ptr<dynasp::IDynAspCountingSolution> solution(
			static_cast<dynasp::IDynAspCountingSolution *>(s.solve(*instance)));

	std::cout << " done." << std::endl;
	std::cout << "OPTIMAL WEIGHT: " << solution->optimalWeight() << std::endl;
	std::cout << "SOLUTION COUNT: " << solution->count() << std::endl;

	return EXIT_SUCCESS;
}
