#include <memory>
#include <cmath>
#include <algorithm>
#include <memory_resource>

#include <string>
using std::pmr::string;
inline string operator ""_s(const char *_Str, size_t _Len) {
	return string{ _Str, _Len };
}

#include <string_view>
using namespace std::literals::string_view_literals;
using std::string_view;

#include<vector>
using std::pmr::vector;

#include<iostream>
#include<fstream>
#include<sstream>

#include<set>
#include<regex>

namespace {
	class PublishTable {
	public:
		class Row {
		public:
			string $Type$;//$型号$/**/;
			string $Number$;//$数量$/**/;
			string $OutTime$;//$发放时间$/**/;
			string $InTime$;//$交回时间$/**/;
			string $UnitName$;//$交回单位$/**/;
			string $DestroyReason$; //$销毁原因$/**/;
		};

		class Page {
		public:
			int $Index$;
			std::vector< Row > $Table$;
		};

		class Article {
		public:
			std::vector< Page > $Pages$;
			inline void clear() { $Pages$.clear(); }
		};
		constexpr static const auto $LineNumber = 68;
		string $FileName;
		string $OutputFileName;
		std::vector< string > $FileLines;
		Article $Article;

		inline PublishTable(string && argFileName, string &&argOutputFileName) :
			$FileName{ std::move(argFileName) },
			$OutputFileName{ std::move(argOutputFileName) } {}
		inline bool readFileData();
		inline bool makeArticle();
		inline bool publish();
	public:
		static inline string trim(string_view arg);
		static vector<string> split(string_view arg);
	};

	inline bool PublishTable::publish() {
		std::ofstream varOStream;
		varOStream.open($OutputFileName.c_str(), std::ios::binary | std::ios::out);
		auto & varData = varOStream;

		varData << u8R"!!~~!!(
\documentclass[hyperref,UTF8]{ctexart}

%设置页面
\usepackage[
	a4paper,
	left=0.9cm,
	right=0.9cm,
	top=1cm,
	bottom=0.5cm]
{geometry}

%设置输出pdf格式
\usepackage[
	colorlinks=true ,
	%bookmarks=true,
	%bookmarksopen=false,
	%pdfpagemode=FullScreen,
	%pdfstartview=Fit,
	pdftitle={蛊真人} ,%标题
	pdfauthor={蛊真人} ,%作者
	pdfsubject={蛊真人} ,%主题
	pdfkeywords={蛊真人} , %关键字
	linkcolor=black 
]{hyperref}  

\usepackage{graphicx}
\usepackage{float}
\usepackage{amsmath}
\usepackage{cite}
\usepackage{caption}
%\usepackage{titlesec}
\usepackage{setspace}

%页码 页眉 页脚
\pagestyle{empty}

%常量
 
%表格相关
\usepackage{array}
\usepackage{makecell}
\usepackage{tabularx}

\newcolumntype{Y}{>{\raggedleft\arraybackslash}X}

\begin{document}
%\begin{spacing}{1.0}%%行间距变为single-space
 
)!!~~!!"sv;

		for (const auto & varPage : $Article.$Pages$) {
			std::size_t varRowNumber = 0;
			const constexpr auto HalfLineNumber = $LineNumber / 2;
			varData << u8R"(
%标题
\centerline{
	\CJKfamily{zhhei}%黑体
	\zihao{2}
	图纸回收记录
}

 %编号
\rightline{
	\CJKfamily{zhsong}%宋体
	\zihao{3}
	编号：\hspace{3.5cm}
}
%%%%%%%%%%%%%%%%%%%%%%

%\begin{center}
\normalsize %设置字体大小
\renewcommand\arraystretch{1.25} %设置行距
\setlength{\tabcolsep}{2pt} %设置单元格横向间隙
\setlength{\arraycolsep}{2pt} %设置单元格横向间隙
\noindent %取消首行缩进
\begin{tabularx}{\textwidth}
{
|p{2em}<{\centering}
|Y|p{1.1em}<{\centering}|p{5.1em}<{\centering}|p{5.1em}<{\centering}|p{3.1em}<{\centering}|p{2.1em}<{\centering}
|Y|p{1.1em}<{\centering}|p{5.1em}<{\centering}|p{5.1em}<{\centering}|p{3.1em}<{\centering}|p{2.1em}<{\centering}|
}
\hline 
\makecell{序\\号}&
\makecell{型号}&\makecell{数\\量}&\makecell{发放\\时间}&\makecell{交回\\时间}&\makecell{交回\\单位}&\makecell{销毁\\原因}&
\makecell{型号}&\makecell{数\\量}&\makecell{发放\\时间}&\makecell{交回\\时间}&\makecell{交回\\单位}&\makecell{销毁\\原因}\\
\hline 
 
)";
			for (; varRowNumber < HalfLineNumber; ++varRowNumber) {

				varData << (1 + varRowNumber);
				varData << char('&');
				varData << std::endl;

				if (varRowNumber < varPage.$Table$.size()) {
					const auto & varR0 = varPage.$Table$[varRowNumber];
					varData << varR0.$Type$ << char('&');
					varData << varR0.$Number$ << char('&');
					varData << varR0.$OutTime$ << char('&');
					varData << varR0.$InTime$ << char('&');
					varData << varR0.$UnitName$ << char('&');
					varData << varR0.$DestroyReason$ << char('&');
					varData << std::endl;
				}
				else {
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << std::endl;
				}

				if ((varRowNumber + HalfLineNumber) < varPage.$Table$.size()) {
					const auto & varR1 = varPage.$Table$[varRowNumber + HalfLineNumber];
					varData << varR1.$Type$ << char('&');
					varData << varR1.$Number$ << char('&');
					varData << varR1.$OutTime$ << char('&');
					varData << varR1.$InTime$ << char('&');
					varData << varR1.$UnitName$ << char('&');
					varData << varR1.$DestroyReason$ << u8R"(\\
\hline )"sv;
					varData << std::endl;
				}
				else {
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << char('&');
					varData << char(' ') << u8R"(\\
\hline )"sv;
					varData << std::endl;
				}
			}

			varData << u8R"(
%%%%%%%%%%%%%%%%%%%%%%Endl 
\hline 

\end{tabularx}
%\end{center}
%\end{table}

)";

			/*\begin{center}
			\small
			1 / 1
			\end{center}*/
			varData << u8R"(
\begin{center}
\small
)";
			varData << varPage.$Index$;
			varData << u8R"( / )";
			varData << $Article.$Pages$.size();
			varData << u8R"(
\end{center}
)";
			varData << u8R"(
\newpage
)" << std::endl;
		}

		varData << u8R"(
%\end{spacing}
\end{document}

)" << std::endl;


		return true;
	}

	inline bool PublishTable::makeArticle() {
		const auto varPageNumberDetail = std::div($FileLines.size(), $LineNumber);
		const auto varPageNumber = varPageNumberDetail.quot + (varPageNumberDetail.rem > 0);
		$Article.clear();
		$Article.$Pages$.reserve(varPageNumber);
		int varPageIndex = 1;
		int varPageTableLineNumber = 0;
		Page * varCurrentPage = nullptr;
		for (const auto & varL : $FileLines) {
			{
				if (varPageTableLineNumber == 0) {
					varCurrentPage = &$Article.$Pages$.emplace_back();
					varCurrentPage->$Index$ = varPageIndex++;
					varCurrentPage->$Table$.reserve($LineNumber);
				}
				++varPageTableLineNumber;
				if (varPageTableLineNumber == $LineNumber) {
					varPageTableLineNumber = 0;
				}
			}
			auto varLineData = split(varL);
			if (varLineData.size() < 5) { continue; }
			auto & varRow = varCurrentPage->$Table$.emplace_back();
			varRow.$Type$ = std::move(varLineData[0]);
			varRow.$Number$ = std::move(varLineData[1]);
			varRow.$OutTime$ = std::move(varLineData[2]);
			varRow.$InTime$ = std::move(varLineData[3]);
			varRow.$UnitName$ = std::move(varLineData[4]);
			varRow.$DestroyReason$ = std::move(varLineData[5]);
		}
		return true;
	}

	inline bool PublishTable::readFileData() {
		$FileLines.clear();
		std::ifstream varFile;
		varFile.open($FileName.c_str(), std::ios::binary | std::ios::in);
		if (varFile.is_open() == false) {
			return false;
		}

		string varFileDataAll;
		constexpr const static auto B = 1024;
		char Data[B];
		{
			auto printError = []() {
				std::cout << "With Out UTF8 BOM" << std::endl;
			};
			varFile.read(Data, 3);
			if (varFile.gcount() != 3) {
				printError();
				return false;
			}
			if ((0x00FF & Data[0]) != 0x00EF) { printError(); return false; }
			if ((0x00FF & Data[1]) != 0x00BB) { printError(); return false; }
			if ((0x00FF & Data[2]) != 0x00BF) { printError(); return false; }
		}
		while (!varFile.eof()) {
			varFile.read(Data, B);
			varFileDataAll.append(Data, static_cast<std::size_t>(varFile.gcount()));
		}

		const static std::regex varR(u8R"([\r\n]+)");
		std::cregex_token_iterator varI{ varFileDataAll.data(),
			varFileDataAll.data() + varFileDataAll.size(),
			varR, -1 };
		std::cregex_token_iterator varE;

		string varLine;
		for (; varI != varE; ++varI) {
			varLine = trim(string_view(varI->first, varI->length()));
			if (varLine.empty()) { continue; }
			$FileLines.push_back(std::move(varLine));
		}

		return !$FileLines.empty();
	}

	inline string PublishTable::trim(string_view arg) {
		const static std::pmr::set<char> isRK{ ' ','\t','\r','\n' };
		auto varB = arg.begin();
		auto varE = arg.end();
		for (; varB != varE; ++varB) {
			if (isRK.count(*varB) > 0) {
				continue;
			}break;
		}
		if (varB == varE) {
			return {};
		}
		for (--varE; varB != varE; --varE) {
			if (isRK.count(*varE) > 0) {
				continue;
			}break;
		}
		return { varB,varE + 1 };
	}

	vector<string> PublishTable::split(string_view arg) {
		const static std::regex varR(u8R"([	 ]+)");
		std::cregex_token_iterator varI{ arg.data(), arg.data() + arg.size(), varR, -1 };
		std::cregex_token_iterator varE;
		vector<string> varAns;
		for (; varI != varE; ++varI) {
			varAns.emplace_back((*varI).first, (*varI).length());
		}
		return std::move(varAns);
	}

}

extern void publishTable(std::string_view a, std::string_view b) {
	PublishTable var(string{ a }, string{ b });
	if (var.readFileData())
		if (var.makeArticle())
			var.publish();
}











