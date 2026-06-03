#pragma once

enum TokenType {
    // Keywords
    Class, 
    Print, Def, Return,
    While, Break, Continue,
    If, Elif, Else,
    True, False, None,
    And, Or, Not,
    Pass,
    
    // Operators
    Plus, Minus, 
    Star, DoubleStar, 
    Slash, DoubleSlash, Mod,
    Pipe, Caret, Ampersand, Tilde,
    LeftShift, RightShift,  
    Less, LessEqual,
    Greater, GreaterEqual,
    Equals, EqualEqual,
    Bang, BangEqual,
    PlusEqual, MinusEqual,
    StarEqual, SlashEqual, ModEqual,
    AndEqual, OrEqual, XorEqual,
    LeftShiftEqual, 
    RightShiftEqual,
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
