#pragma once

enum TokenType {
    // Keywords
    Print, 
    While, If, Else,
    True, False, None,
    And, Or, Not,
    
    // Operators and punctuators
    Plus, Minus, Star, Slash, 
    Less, LessEqual,
    Greater, GreaterEqual,
    Equals, EqualEqual,
    Bang, BangEqual,
    Dot, 
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    Comma, Colon, Semicolon,
    Indent, Dedent, 
    Newline, 
    EndOfFile,
    
    // Identifiers and literals
    Name, Number, String,
    
    // Others
    Error,
};
