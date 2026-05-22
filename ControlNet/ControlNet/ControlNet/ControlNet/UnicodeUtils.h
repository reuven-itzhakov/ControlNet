#pragma once
#include <windows.h>
#include <string>

// Enhanced Unicode utilities for handling complex characters like emojis
class UnicodeUtils {
public:
    // Get Unicode input more reliably, handling complex sequences
    static std::wstring GetUnicodeFromKeyboard(DWORD vkCode, DWORD scanCode, BYTE* keyboardState, HKL keyboardLayout);
    
    // Check if a character/sequence is a printable Unicode character (including emojis)
    static bool IsPrintableUnicode(const std::wstring& text);
    
    // Validate if a wide string contains valid Unicode sequences
    static bool IsValidUnicodeSequence(const std::wstring& text);
    
    // Get the actual character count (considering surrogate pairs and combining sequences)
    static size_t GetUnicodeCharacterCount(const std::wstring& text);
    
    // Safe UTF-8 conversion that handles all Unicode ranges
    static std::string SafeWStringToUTF8(const std::wstring& wstr);
    static std::wstring SafeUTF8ToWString(const std::string& str);

private:
    // Helper to check if a code unit is a high surrogate
    static bool IsHighSurrogate(wchar_t ch);
    // Helper to check if a code unit is a low surrogate  
    static bool IsLowSurrogate(wchar_t ch);
    // Helper to check if a code point is an emoji or symbol
    static bool IsEmojiCodePoint(uint32_t codePoint);
};