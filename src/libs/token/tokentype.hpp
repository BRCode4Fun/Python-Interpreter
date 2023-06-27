#pragma once

enum TokenType {
    // Keywords
    Print, 
    While, If, Elif, Else,
    True, False, None,
    And, Or, Not, Def,
    
    // Operators and punctuators
    Plus, Minus, Star, Slash, Mod,
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
    Int, Float, Name, String,
    
    // Others
    Error,
};
