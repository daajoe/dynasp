
//FIXME: remove debug code below here
template<typename T>
static void printColl(const T &coll)
{
	std::cout << "[";
	const char *sep = "";
	for(const auto o : coll)
	{
		std::cout << sep << o;
		sep = ",";
	}
	std::cout << "]";
}

//FIXME: remove debug code below here
template<typename T>
static void printMap(const T &coll)
{
	std::cout << "[";
	const char *sep = "";
	for(const auto o : coll)
	{
		std::cout << sep << o.first << ":" << o.second;
		sep = ",";
	}
	std::cout << "]";
}

//FIXME: remove debug code below here
template<typename T>
static void printRules(const T &coll)
{
	std::cout << "[";
	const char *sep = "";
	for(const auto o : coll)
	{
		std::cout << sep << o.first << ":{" << o.second.minBodyWeight;
		std::cout << "." << o.second.maxBodyWeight;
		std::cout << "." << o.second.seenHeadAtoms << "}";
		sep = ",";
	}
	std::cout << "]";
}


//FIXME: remove debug code below here
template<typename T>
static void printCert(const T &coll)
{
	std::cout << "[";
	const char *sep = "";
	for(const auto o : coll)
	{
		std::cout << sep;
		printColl(o.atoms);
		std::cout << (o.same ? "+" : ":");
		printRules(o.rules);
		sep = ",";
	}
	std::cout << "]";
}

