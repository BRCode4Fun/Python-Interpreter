#pragma once

enum TokenType {
    // Keywords
    Print, Def,
    While, If, Elif, Else,
    Return,
    True, False, None,
    And, Or, Not,
    
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
