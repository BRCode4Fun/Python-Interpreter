#pragma once

enum TokenType {
    // Keywords
    Print,
    
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