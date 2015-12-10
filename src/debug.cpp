
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

