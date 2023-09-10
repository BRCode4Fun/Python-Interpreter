#pragma once

enum TokenType {
    // Keywords
    Print, Def, Return,
    While, Break, Continue,
    If, Elif, Else,
    True, False, None,
    And, Or, Not,
    
    // Operators
    Plus, Minus, 
    Star, DoubleStar, Slash, Mod,
    Pipe, Caret, Ampersand, Tilde,
    LeftShift, RightShift,  
    Less, LessEqual,
    Greater, GreaterEqual,
    Equals, EqualEqual,
    Bang, BangEqual,
    Dot,
    
    // Punctuators
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
