//
// Created by hecher on 5/18/16.
//

#ifndef DYNASP_2ND_LEVEL_EXTENSIONS_REFERENCECOUNTER_HPP
#define DYNASP_2ND_LEVEL_EXTENSIONS_REFERENCECOUNTER_HPP


class ReferenceCounter
{
	public:
		virtual ~ReferenceCounter() { }
		ReferenceCounter() : refCnt_(1) { }
		void increase() { ++refCnt_; };
		void decrease() { --refCnt_; if (refCnt_ == 0) delete this; };
		bool timeToDie() { return refCnt_ <= 0; };
	private:
		size_t refCnt_;

};


#endif //DYNASP_2ND_LEVEL_EXTENSIONS_REFERENCECOUNTER_HPP
