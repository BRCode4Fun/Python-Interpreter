#pragma once

enum TokenType {
    // Keywords
    Print, While, If, 
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
    EndOfFile,
    
    // Identifiers and literals
    Identifier, Number,
    
    // Others
    Error,
};
