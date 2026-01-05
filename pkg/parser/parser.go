// Package parser provides an S-expression parser for the apertures language.
package parser

import (
	"fmt"
	"strconv"
	"strings"
	"unicode"

	"github.com/queelius/aperture/pkg/ast"
)

// Parser holds the state for parsing.
type Parser struct {
	input  string
	pos    int
	line   int
	column int
}

// New creates a new parser for the given input.
func New(input string) *Parser {
	return &Parser{
		input:  input,
		pos:    0,
		line:   1,
		column: 1,
	}
}

// Parse parses the input and returns all top-level expressions.
func Parse(input string) ([]ast.Expr, error) {
	p := New(input)
	return p.ParseAll()
}

// ParseOne parses a single expression from the input.
func ParseOne(input string) (ast.Expr, error) {
	p := New(input)
	return p.ParseExpr()
}

// ParseAll parses all expressions in the input.
func (p *Parser) ParseAll() ([]ast.Expr, error) {
	var exprs []ast.Expr
	for {
		p.skipWhitespaceAndComments()
		if p.eof() {
			break
		}
		expr, err := p.ParseExpr()
		if err != nil {
			return nil, err
		}
		exprs = append(exprs, expr)
	}
	return exprs, nil
}

// ParseExpr parses a single expression.
func (p *Parser) ParseExpr() (ast.Expr, error) {
	p.skipWhitespaceAndComments()
	if p.eof() {
		return nil, p.error("unexpected end of input")
	}

	ch := p.peek()

	// Quote shorthand
	if ch == '\'' {
		return p.parseQuote()
	}

	// List
	if ch == '(' {
		return p.parseList()
	}

	// Hole
	if ch == '?' {
		return p.parseHole()
	}

	// String
	if ch == '"' {
		return p.parseString()
	}

	// Number (including negative)
	if unicode.IsDigit(ch) || (ch == '-' && p.peekNext() != 0 && unicode.IsDigit(p.peekNext())) {
		return p.parseNumber()
	}

	// Symbol (including keywords like true, false, nil)
	if isSymbolStart(ch) {
		return p.parseSymbol()
	}

	return nil, p.error(fmt.Sprintf("unexpected character: %c", ch))
}

func (p *Parser) parseQuote() (ast.Expr, error) {
	pos := p.currentPos()
	p.advance() // consume '
	expr, err := p.ParseExpr()
	if err != nil {
		return nil, err
	}
	return ast.Quote(pos, expr), nil
}

func (p *Parser) parseList() (ast.Expr, error) {
	pos := p.currentPos()
	p.advance() // consume (

	var elements []ast.Expr
	for {
		p.skipWhitespaceAndComments()
		if p.eof() {
			return nil, p.error("unclosed list, expected )")
		}
		if p.peek() == ')' {
			p.advance() // consume )
			break
		}
		expr, err := p.ParseExpr()
		if err != nil {
			return nil, err
		}
		elements = append(elements, expr)
	}
	return ast.List(pos, elements...), nil
}

func (p *Parser) parseHole() (ast.Expr, error) {
	pos := p.currentPos()
	p.advance() // consume ?

	// Parse the hole name (identifier or namespace.identifier)
	if p.eof() || !isSymbolStart(p.peek()) {
		return nil, p.error("expected hole name after ?")
	}

	name := p.readSymbol()

	// Check for namespace.name pattern
	if p.peek() == '.' {
		p.advance() // consume .
		if p.eof() || !isSymbolStart(p.peek()) {
			return nil, p.error("expected name after namespace.")
		}
		localName := p.readSymbol()
		return ast.NamespacedHole(pos, name, localName), nil
	}

	return ast.Hole(pos, name), nil
}

func (p *Parser) parseString() (ast.Expr, error) {
	pos := p.currentPos()
	p.advance() // consume opening "

	var sb strings.Builder
	for {
		if p.eof() {
			return nil, p.error("unclosed string")
		}
		ch := p.peek()
		if ch == '"' {
			p.advance() // consume closing "
			break
		}
		if ch == '\\' {
			p.advance()
			if p.eof() {
				return nil, p.error("unclosed string after escape")
			}
			escaped := p.peek()
			p.advance()
			switch escaped {
			case 'n':
				sb.WriteByte('\n')
			case 't':
				sb.WriteByte('\t')
			case 'r':
				sb.WriteByte('\r')
			case '\\':
				sb.WriteByte('\\')
			case '"':
				sb.WriteByte('"')
			default:
				sb.WriteRune(escaped)
			}
		} else {
			sb.WriteRune(ch)
			p.advance()
		}
	}
	return ast.Str(pos, sb.String()), nil
}

func (p *Parser) parseNumber() (ast.Expr, error) {
	pos := p.currentPos()
	start := p.pos

	// Optional negative sign
	if p.peek() == '-' {
		p.advance()
	}

	// Integer part
	for !p.eof() && unicode.IsDigit(p.peek()) {
		p.advance()
	}

	// Optional decimal part
	if p.peek() == '.' {
		p.advance()
		for !p.eof() && unicode.IsDigit(p.peek()) {
			p.advance()
		}
	}

	// Optional exponent
	if p.peek() == 'e' || p.peek() == 'E' {
		p.advance()
		if p.peek() == '+' || p.peek() == '-' {
			p.advance()
		}
		for !p.eof() && unicode.IsDigit(p.peek()) {
			p.advance()
		}
	}

	numStr := p.input[start:p.pos]
	val, err := strconv.ParseFloat(numStr, 64)
	if err != nil {
		return nil, p.error(fmt.Sprintf("invalid number: %s", numStr))
	}
	return ast.Num(pos, val), nil
}

func (p *Parser) parseSymbol() (ast.Expr, error) {
	pos := p.currentPos()
	name := p.readSymbol()

	// Check for keywords
	switch name {
	case "nil":
		return ast.Nil(pos), nil
	case "true":
		return ast.Bool(pos, true), nil
	case "false":
		return ast.Bool(pos, false), nil
	default:
		return ast.Sym(pos, name), nil
	}
}

func (p *Parser) readSymbol() string {
	start := p.pos
	for !p.eof() && isSymbolChar(p.peek()) {
		p.advance()
	}
	return p.input[start:p.pos]
}

// Helper methods

func (p *Parser) eof() bool {
	return p.pos >= len(p.input)
}

func (p *Parser) peek() rune {
	if p.eof() {
		return 0
	}
	return rune(p.input[p.pos])
}

func (p *Parser) peekNext() rune {
	if p.pos+1 >= len(p.input) {
		return 0
	}
	return rune(p.input[p.pos+1])
}

func (p *Parser) advance() {
	if !p.eof() {
		if p.input[p.pos] == '\n' {
			p.line++
			p.column = 1
		} else {
			p.column++
		}
		p.pos++
	}
}

func (p *Parser) currentPos() ast.Pos {
	return ast.Pos{Line: p.line, Column: p.column}
}

func (p *Parser) skipWhitespaceAndComments() {
	for !p.eof() {
		ch := p.peek()
		if unicode.IsSpace(ch) {
			p.advance()
		} else if ch == ';' {
			// Skip line comment
			for !p.eof() && p.peek() != '\n' {
				p.advance()
			}
		} else {
			break
		}
	}
}

func (p *Parser) error(msg string) error {
	return fmt.Errorf("parse error at %d:%d: %s", p.line, p.column, msg)
}

// Character classification

func isSymbolStart(ch rune) bool {
	return unicode.IsLetter(ch) || ch == '_' || ch == '+' || ch == '-' ||
		ch == '*' || ch == '/' || ch == '<' || ch == '>' || ch == '=' ||
		ch == '!' || ch == '&' || ch == '|' || ch == '^' || ch == '%'
}

func isSymbolChar(ch rune) bool {
	return isSymbolStart(ch) || unicode.IsDigit(ch) || ch == '?'
}
