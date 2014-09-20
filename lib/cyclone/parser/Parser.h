#ifndef CYCLONE_PARSER_PARSER_H__
#define CYCLONE_PARSER_PARSER_H__

#include <cyclone/syntaxtree/Token.h>
#include <cyclone/syntaxtree/Node.h>

namespace cyclone {
namespace parser {

template<class R>
class RuleBase {
public:

	virtual ~RuleBase () { }
};

template<class R>
class RepeatedRule;

template<class A, class B>
class ConcatenatedRule;

template<class A, class B>
class AlternateRule;

template<class Dest>
class RuleName;

template<class R>
class RepeatedRule : public RuleBase<RepeatedRule<R>> {
public:

	RepeatedRule (const R & rule, unsigned min, unsigned max)
		: m_rule (rule), m_min (min), m_max (max) {
	}

	template<class B>
	ConcatenatedRule<RepeatedRule<R>, B> operator + (const B & b) const {
		return ConcatenatedRule<RepeatedRule<R>, B> (*this, b);
	}

	template<class B>
	AlternateRule<RepeatedRule<R>, B> operator | (const B & b) const {
		return AlternateRule<RepeatedRule<R>, B> (*this, b);
	}

private:

	R			m_rule;
	unsigned	m_min;
	unsigned	m_max;
};

template<class A, class B>
class AlternateRule : public RuleBase<AlternateRule<A, B>> {
public:

	AlternateRule (const A & left, const B & right)
		: m_left (left), m_right (right) {
	}

	RepeatedRule<AlternateRule<A, B>> operator () (unsigned count) const {
		return RepeatedRule<AlternateRule<A, B>> (*this, count, count);
	}

	RepeatedRule<AlternateRule<A, B>> operator () (unsigned min, unsigned max) const {
		return RepeatedRule<AlternateRule<A, B>> (*this, min, max);
	}

	template<class NewB>
	ConcatenatedRule<AlternateRule<A, B>, NewB> operator + (const NewB & b) const {
		return ConcatenatedRule<AlternateRule<A, B>, NewB> (*this, b);
	}

	template<class NewB>
	AlternateRule<AlternateRule<A, B>, NewB> operator | (const NewB & b) const {
		return AlternateRule<AlternateRule<A, B>, NewB> (*this, b);
	}

private:

	A	m_left;
	B	m_right;
};


template<class A, class B>
class ConcatenatedRule : public RuleBase<ConcatenatedRule<A, B>> {
public:

	ConcatenatedRule (const A & left, const B & right)
		: m_left (left), m_right (right) {
	}

	RepeatedRule<ConcatenatedRule<A, B>> operator () (unsigned count) const {
		return RepeatedRule<ConcatenatedRule<A, B>> (*this, count, count);
	}

	RepeatedRule<ConcatenatedRule<A, B>> operator () (unsigned min, unsigned max) const {
		return RepeatedRule<ConcatenatedRule<A, B>> (*this, min, max);
	}

	template<class NewB>
	ConcatenatedRule<ConcatenatedRule<A, B>, NewB> operator + (const NewB & b) const {
		return ConcatenatedRule<ConcatenatedRule<A, B>, NewB> (*this, b);
	}

	template<class NewB>
	AlternateRule<ConcatenatedRule<A, B>, NewB> operator | (const NewB & b) const {
		return AlternateRule<ConcatenatedRule<A, B>, NewB> (*this, b);
	}

private:

	A	m_left;
	B	m_right;
};

class TerminalRule : public RuleBase<TerminalRule> {
public:

	TerminalRule (cyclone::syntaxtree::TokenType tokenType)
		: m_tokenType (tokenType) {
	}

	cyclone::syntaxtree::TokenType tokenType () const {
		return m_tokenType;
	}

	RepeatedRule<TerminalRule> operator () (unsigned count) const {
		return RepeatedRule<TerminalRule> (*this, count, count);
	}

	RepeatedRule<TerminalRule> operator () (unsigned min, unsigned max) const {
		return RepeatedRule<TerminalRule> (*this, min, max);
	}

	template<class B>
	ConcatenatedRule<TerminalRule, B> operator + (const B & b) const {
		return ConcatenatedRule<TerminalRule, B> (*this, b);
	}

	template<class B>
	AlternateRule<TerminalRule, B> operator | (const B & b) const {
		return AlternateRule<TerminalRule, B> (*this, b);
	}

private:

	cyclone::syntaxtree::TokenType m_tokenType;
};

template<class Dest, class R>
class NamedRule : public RuleBase<NamedRule<Dest, R>> {
public:

	NamedRule (const RuleName<Dest> & name, const R & rule) : m_name (name), m_rule (rule) {
	}

private:

	RuleName<Dest>	m_name;
	R				m_rule;
};

template<class Dest>
class RuleName {
public:

	RuleName (const std::u16string & name) : m_name (name) {
	}

	template<class R>
	NamedRule<Dest, R> operator > (const R & rule) const {
		return NamedRule<Dest, R> (*this, rule);
	}

private:

	std::u16string	m_name;
};

inline TerminalRule token (cyclone::syntaxtree::TokenType tokenType) {
	return TerminalRule (tokenType);
}

template<class Dest>
inline RuleName<Dest> rule (const std::u16string & name) {
	return RuleName<Dest> (name);
}

template<class Dest, class R>
class Parser {
public:

	Parser (const NamedRule<Dest, R> & rule) : m_rule (rule) {
	}

private:

	NamedRule<Dest, R>	m_rule;
};

template<class Dest, class R>
inline Parser<Dest, R> parser (const NamedRule<Dest, R> & rule) {
	return Parser<Dest, R> (rule);
}

}	// namespace parser
}	// namespace cyclone

#endif	// CYCLONE_PARSER_PARSER_H__
