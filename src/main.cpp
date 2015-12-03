#include <dynasp/main>
using namespace dynasp;

#include <sharp/main>
using namespace sharp;

#include <htd/main>
using namespace htd;

#include <iostream>
using namespace std;

int main()
{
	IGroundAspInstance *i = Parser::get()->parse(&std::cin);

	MinFillOrderingAlgorithmImpl o;
	BucketEliminationTreeDecompositionAlgorithm a(o);

	DynAspAlgorithm da;
	IterativeTreeTupleSolver s(a, da);

	s.solve(*i);

	return 0;
}
