#ifndef DYNASP_UTIL_DEBUG_H_
#define DYNASP_UTIL_DEBUG_H_

//#define DEBUG
#ifdef DEBUG

#include <iostream>

namespace dynasp
{
	class Debug
	{
	public:
		static std::ostream &stream;

		template<typename T>
		static void printColl(std::ostream &out, const T &coll)
		{
			out << "[";
			const char *sep = "";
			for(const auto o : coll)
			{
				out << sep << o;
				sep = ",";
			}
			out << "]";
		}

		template<typename T>
		static void printMap(std::ostream &out, const T &coll)
		{
			out << "[";
			const char *sep = "";
			for(const auto o : coll)
			{
				out << sep << o.first << ":" << o.second;
				sep = ",";
			}
			out << "]";
		}

		template<typename T>
		static void printRuleMap(std::ostream &out, const T &coll)
		{
			out << "[";
			const char *sep = "";
			for(const auto o : coll)
			{
				out << sep << o.first << ":{" << o.second.minBodyWeight;
				out << "." << o.second.maxBodyWeight;
				out << "." << o.second.seenHeadAtoms << "}";
				sep = ",";
			}
			out << "]";
		}

		template<typename T>
		static void printCert(std::ostream &out, const T &coll)
		{
			out << "[";
			const char *sep = "";
			for(const auto o : coll)
			{
				out << sep;
				printColl(out, o.atoms);
				out << (o.same ? "+" : ":");
				printRuleMap(out, o.rules);
				sep = ",";
			}
			out << "]";
		}
		
		template<typename T>
		static void printSimpleCert(std::ostream &out, const T &coll)
		{
			out << "[";
			const char *sep = "";
			for(const auto o : coll)
			{
				out << sep;
				out << (o.same ? "+" : ":");
				printColl(out, o.atoms);
				sep = ",";
			}
			out << "]";
		}
	
		template<typename T>
		static void printInverseCert(std::ostream &out, const T &coll)
		{
			out << "[";
			const char *sep = "";
			for(const auto o : coll)
			{
				out << sep;
				printColl(out, o.atoms);
				sep = ",";
			}
			out << "]";
		}

		template<typename T>
		static void printRuleSetCert(std::ostream &out, const T &coll)
		{
			out << "[";
			const char *sep = "";
			for(const auto o : coll)
			{
				out << sep;
				printColl(out, o.atoms);
				out << (o.same ? "+" : ":");
				printColl(out, o.rules);
				sep = ",";
			}
			out << "]";
		}

	private:
		Debug();
	};

} // namespace dynasp

#define DBG_SET(x) \
	Debug::stream = (x)

#define DBG(x) \
	Debug::stream << (x)

#define DBG_COLL(x) \
	Debug::printColl(Debug::stream, (x))

#define DBG_MAP(x) \
	Debug::printMap(Debug::stream, (x))

#define DBG_RULMAP(x) \
	Debug::printRuleMap(Debug::stream, (x))

#define DBG_CERT(x) \
	Debug::printCert(Debug::stream, (x))

#define DBG_SCERT(x) \
	Debug::printSimpleCert(Debug::stream, (x))

#define DBG_ICERT(x) \
	Debug::printInverseCert(Debug::stream, (x))

#define DBG_RSCERT(x) \
	Debug::printRuleSetCert(Debug::stream, (x))

#else // !DEBUG

#define DBG_SET(...)
#define DBG(...)
#define DBG_COLL(...)
#define DBG_MAP(...)
#define DBG_RULMAP(...)
#define DBG_CERT(...)
#define DBG_SCERT(...)
#define DBG_ICERT(...)
#define DBG_RSCERT(...)

#endif // DEBUG

#endif // DYNASP_UTIL_DEBUG_H_
