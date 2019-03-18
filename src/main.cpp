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
#include <dynasp/CertificateDynAspTuple.hpp>
#include <dynasp/ClaspAlgorithm.hpp>
#include <csignal>
#include <dynasp/DependencyGraphEvaluator.hpp>
#include <dynasp/JoinEvaluator.hpp>
#include <dynasp/CombinedEvaluator.hpp>
#include <dynasp/DynAspSupportedAlgorithm.hpp>
#include <dynasp/DynAspProjectionAlgorithm.hpp>

#ifdef HAVE_UNISTD_H 
	#include <unistd.h>
#else
//FIXME: what if we don't have unistd.h for getopt?
#endif

DYNASP_HELPER_EXPORT htd::LibraryInstance* inst = NULL;
//htd::createManagementInstance(0);

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
			while((opt = getopt(argc, argv, "P:YXzq:yi:xmp:o:rnSvhbds:c:t:l:")) != -1)
				switch(opt)
				{
				case 'P':
					this->paceOut = optarg;
					break;
				case 'Y':
					this->projection = true;
					break;
				case 'X':
					this->supported = true;
					break;
				case 'z':
					this->qbfoutput = true;
					break;
				case 'y':
					this->saveReductModels = true;
					break;
				case 'S':
					this->satonly = true;
					this->passes = 1;
					break;
				case 'q':
					this->folder = optarg;
					break;
				case 'x':
					this->convert2Clasp = true;
					this->passes = 1;
					break;
				case 'i':	//heurIstIc
					this->heur_mode = (unsigned)strtol(optarg, NULL, 10);
					break;
				case 'o':
					this->tdopt = (unsigned)strtol(optarg, NULL, 10);
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
					#ifdef CACHE_CERTS
						if (this->passes >= 2)
							this->children = 2;
					#endif

					if (this->passes < 3)
						this->compr = false;
					break;
				case 'm':
					this->compr = false;
					//this->passes = 3;
					break;
				case 'l':
					if (!this->weak
					#ifdef CACHE_CERTS
						|| (this->passes >= 2)
					#endif
					)
						this->children = 2;
					else
						this->children = (unsigned)strtol(optarg, NULL, 10);
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
					if (this->useSeed)
						this->pid = (unsigned)strtol(optarg, NULL, 10);
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

						//this->reductSpeedup = false;
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
		bool qbfoutput = false;
		bool supported = false;
		bool projection = false;
		bool displayVersion = false; // -v, --version
		bool reductSpeedup = true; // -v, --version
		unsigned tdopt = 0; // -v, --version
		bool convert2Clasp = false; // -v, --version
		bool saveReductModels = false;
		bool satonly = false; // -v, --version
		bool displayHelp = false; // -h, --help
		bool printBenchmarks = false; // -b, --benchmark
		bool printMachineReadable = false; // 2x -b, --benchmark
		bool decompositionOnly = false; // -d, --decomp
		bool customTreeDecomposition = false; // -t, --tree
		DecompositionHeuristic heuristic = MINIMUM_FILL_EDGES;
		bool useSeed = false; // -s, --seed
		unsigned seed = 0, children = 
		#ifdef CACHE_CERTS
			2
		#else
			3
		#endif
		, passes = 3, heur_mode = 0;
		bool weak = true, compr = true;
		unsigned pid = 0;
		bool readFromFile = false;
		char *fileToRead = nullptr;
		bool customConfiguration = false; // -c <config>, --config=<config>
		std::string folder, paceOut;
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
	printVersion()
	{
		std::cout
			<< PACKAGE_STRING << std::endl
			<< "Copyright (C) 2016 Michael Morak & Markus Hecher" << std::endl
			<< "License GPLv3+: GNU GPL version 3 or later "
				<< "<http://gnu.org/licenses/gpl.html>." << std::endl
			<< "This is free software: you are free to change and "
				<< "redistribute it." << std::endl
			<< "There is NO WARRANTY, to the extent permitted by law."
				<< std::endl
			<< std::endl
			<< "Written by Michael Morak & Markus Hecher." << std::endl;
	}
}

/*void sighandler(int)
{
	sharp::Benchmark::interrupt();
}*/

#ifdef DEBUG_INFO
	dynasp::DynAspAlgorithm* linkingAlgorithm = nullptr;
#endif

void printBenchmarks(int signal)
{
	std::cout << std::endl;

	//for (int i = 0; i < 1000; ++i)
	//printf("asdsdf\n");
	sharp::Benchmark::printBenchmarks(std::cout, false);
#ifdef DEBUG_INFO
	if (linkingAlgorithm)
		linkingAlgorithm->onExit();
	else
		std::cout << "no linking" << std::endl;
#endif

	//abort();
	_exit(signal);
}

void printBenchmarksMachineReadable(int signal)
{
	std::cout << std::endl;
	/*for (int i = 0; i < 1000; ++i)
	printf("asdsdf\n");*/
	sharp::Benchmark::printBenchmarks(std::cout, true);
#ifdef DEBUG_INFO
	if (linkingAlgorithm)
		linkingAlgorithm->onExit();
	else
		std::cout << "no linking" << std::endl;
#endif


	//abort();
	_exit(signal);
}

int main(int argc, char *argv[])
{
	//assert(false);
	inst = htd::createManagementInstance(0);


	//signal(SIGINT, sighandler);
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
		/*signal(SIGABRT, printBenchmarksMachineReadable);
		signal(SIGFPE, printBenchmarksMachineReadable);
		signal(SIGILL, printBenchmarksMachineReadable);*/
		signal(SIGINT, printBenchmarksMachineReadable);
		//signal(SIGSEGV, printBenchmarksMachineReadable);
		signal(SIGTERM, printBenchmarksMachineReadable);
	}
	else if(opts.printBenchmarks)
	{
		/*signal(SIGABRT, printBenchmarks);
		signal(SIGFPE, printBenchmarks);
		signal(SIGILL, printBenchmarks);*/
		signal(SIGINT, printBenchmarks);
		//signal(SIGSEGV, printBenchmarks);
		signal(SIGTERM, printBenchmarks);
	}


	if(opts.useSeed)
	{
		std::srand(opts.seed);
		sharp::Benchmark::setId(opts.pid);
	}

	if(opts.customTreeDecomposition)
	{
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

	if(opts.customConfiguration) dynasp::create::set(opts.configuration);

	dynasp::create::setPaceOut(opts.paceOut);
	dynasp::create::setQBFOutput(opts.qbfoutput);
	dynasp::create::setNon(!opts.weak);
	dynasp::create::setReductSpeedup(opts.reductSpeedup);
	dynasp::create::setPasses(opts.passes);
	dynasp::create::setSatOnly(opts.satonly);
	dynasp::create::setCompr(opts.compr);
	dynasp::create::setSaveReductModels(opts.saveReductModels);
	dynasp::create::setSupported(opts.supported);
	dynasp::create::setProjection(opts.projection);

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
	if (instance.get())
	{
		instance->cleanTmps();
		if (opts.qbfoutput)
		{
			std::ofstream f;
			f.open(opts.folder + ".qcir");
			f << instance->toQBFString();
			f.close();
		}
	}
	sharp::Benchmark::registerTimestamp("parsing time");

	if(!instance.get())
		exit(EXIT_PARSING_ERROR);

	std::cout << "Initializing solver..." << std::endl;
	std::cout << "Using " << opts.passes  << " Passes" << std::endl;

/*	std::unique_ptr<htd::ITreeDecompositionAlgorithm> tdAlgorithm(
			htd::TreeDecompositionAlgorithmFactory::instance()
			.getTreeDecompositionAlgorithm());*/
	std::unique_ptr<htd::ITreeDecompositionAlgorithm> tdAlgorithm(
			inst->treeDecompositionAlgorithmFactory().createInstance());
			//.getTreeDecompositionAlgorithm());

	dynasp::DynAspAlgorithm algorithm;
	dynasp::DynAspCertificateAlgorithm second_layer_algorithm, firstlink;
	linkingAlgorithm = &second_layer_algorithm;
#ifdef SUPPORTED_CHECK
	dynasp::DynAspSupportedAlgorithm supported_algorithm;

	if (opts.supported)
		linkingAlgorithm = &supported_algorithm;

#endif
	dynasp::DynAspProjectionAlgorithm projection_algorithm;
	if (opts.projection)
		linkingAlgorithm = &projection_algorithm;

/*#ifdef DEBUG_INFO
	p_algo = linkingAlgorithm; //&second_layer_algorithm;
#endif*/

	if (opts.passes >= 4)
		linkingAlgorithm->setFurther(true);
	
	std::unique_ptr<dynasp::IDynAspCountingSolutionExtractor> extractor(
			dynasp::create::countingSolutionExtractor());
	std::unique_ptr<sharp::ITreeTupleAlgorithm> two_layered_algorithm(
			sharp::create::treeTupleAlgorithm(algorithm, *linkingAlgorithm));

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
	dynasp::ClaspAlgorithm calgorithm;
	
	if (opts.satonly) {
		solver.reset(sharp::create::treeSolver(*tdAlgorithm, algorithm, *extractor));
	}
	else if (opts.passes >= 2) { //#ifdef SEVERAL_PASSES
		//sharp::TreeTupleAlgorithmVector algs;
		algs.push_back(&algorithm);
		if (opts.passes >= 4)
		{
			//firstlink.setFurther(true); //OnlyModelsAsCounterwitnesses(true);
			algs.push_back(&firstlink);
		}

		if (opts.passes >= 3) { //#ifdef THREE_PASSES
			//dynasp::DynAspAlgorithm algorithm2;
			algorithm2.setFurther(true);

			if (!opts.supported && !opts.projection)
			#ifndef MERGE_PASS_TWO_THREE
				algs.push_back(&algorithm2);
			#endif
		} //#endif
		
		#ifdef MERGE_PASS_TWO_THREE

			if (!opts.supported && !opts.projection)
			{
				/*std::unique_ptr<sharp::ITreeTupleAlgorithm>*/ two_layered_algorithm2.reset(
				sharp::create::teeTupleAlgorithm(algorithm2, *linkingAlgorithm)); //second_layer_algorithm));
				algs.push_back(*two_layered_algorithm2);
			}
			else
			{
				algs.push_back(linkingAlgorithm);

				//two_layered_algorithm2.reset(sharp::create::teeTupleAlgorithm(*linkingAlgorithm, algorithm2)); //second_layer_algorithm));
				//algs.push_back(*two_layered_algorithm2);
			}
		#else
			algs.push_back(linkingAlgorithm); //&second_layer_algorithm);
			/*if (opts.supported)
			{
			#ifndef MERGE_PASS_TWO_THREE
				algs.push_back(&algorithm2);
			#endif
				algs.push_back(&second_layer_algorithm);	
			}*/

		#endif
		/*std::unique_ptr<sharp::ITreeSolver>*/ solver.reset(
				sharp::create::treeSolver(*tdAlgorithm, algs, *extractor));
	} else { 
		if (!opts.convert2Clasp)
	//#else
		/*std::unique_ptr<sharp::ITreeSolver>*/ solver.reset(
				sharp::create::treeSolver(*tdAlgorithm, *two_layered_algorithm, *extractor));
		else
			solver.reset(
			sharp::create::treeSolver(*tdAlgorithm, calgorithm, *extractor));

	} //#endif
#endif

	std::cout << "Decomposing..." << std::endl;
	sharp::ITreeDecompositionEvaluator* eval = nullptr;
	switch (opts.tdopt)
	{
		case 0:
			break;
		case 1:
			eval = new DependencyGraphEvaluator(*instance);
			linkingAlgorithm->setDepEval((DependencyGraphEvaluator*)(eval));
			algorithm2.setDepEval((DependencyGraphEvaluator*)(eval));
			break;
		case 2:
			eval = new JoinEvaluator();
			break;
		default:
			eval = new CombinedEvaluator(opts.tdopt, *instance);
			break;
	}

	sharp::Benchmark::useOrdering(opts.heur_mode); //opts.printMachineReadable);
	sharp::Benchmark::setFolder(opts.folder);
	if (opts.projection)
	{
		std::ofstream fox;
		fox.open(opts.folder);
		fox.close();
	}
	std::unique_ptr<htd::ITreeDecomposition> td(solver->decompose(*instance, opts.weak, opts.children, opts.tdopt, eval));
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

		//mpz_class x = solution->count();
		//mpz_size(solution->count());
		std::cout << "SOLUTION COUNT: " << solution->count() << "@" << mpz_size(solution->count().get_mpz_t()) <<"," << mpz_sizeinbase(solution->count().get_mpz_t(), 10)  << std::endl;
	}

	delete eval;
	if(opts.printBenchmarks)
	{
		std::cout << std::endl;
		sharp::Benchmark::printBenchmarks(std::cout, opts.printMachineReadable);
	}
#ifdef DEBUG_INFO
	linkingAlgorithm->onExit();
#endif
	//delete solver.get();
	_exit(0);

	return EXIT_SUCCESS;
}
