#pragma once
#include <ostream>

#define SOURCEPOS ::lubee::SourcePos{__FILE__, __PRETTY_FUNCTION__, __func__, __LINE__}
namespace lubee {
	//! ソースコード上の位置を表す情報
	struct SourcePos {
		const char	*filename,
					*funcname,
					*funcname_short;
		int			line;
	};
	inline std::ostream& operator << (std::ostream& s, const SourcePos& p) {
		using std::endl;
		return s
			<< "file = " << p.filename << ':' << p.line << endl
			<< "function = " << p.funcname;
	}
}
