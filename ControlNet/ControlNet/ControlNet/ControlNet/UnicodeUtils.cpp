#include "UnicodeUtils.h"
#include <windows.h>
#include <string>

std::wstring UnicodeUtils::GetUnicodeFromKeyboard(DWORD vkCode, DWORD scanCode, BYTE* keyboardState, HKL keyboardLayout) {
    // Use a larger buffer to handle complex Unicode sequences
    WCHAR buffer[16] = { 0 };
    std::wstring result;
    
    // First, try ToUnicodeEx with a larger buffer
    int charCount = ToUnicodeEx(vkCode, scanCode, keyboardState, buffer, 15, 0, keyboardLayout);
    
    if (charCount > 0) {
        // ToUnicodeEx succeeded, process the result
        result = std::wstring(buffer, charCount);
        
        // Validate the Unicode sequence
        if (IsValidUnicodeSequence(result)) {
            return result;
        }
    }
    
    // If ToUnicodeEx failed or gave invalid results, try alternative approach
    // For complex characters like emojis, we might need to handle them differently
    
    // Check if this might be a dead key or complex sequence
    if (charCount == -1) {
        // Dead key detected - might be part of a complex sequence
        // Call again to get the actual character
        charCount = ToUnicodeEx(vkCode, scanCode, keyboardState, buffer, 15, 0, keyboardLayout);
        if (charCount > 0) {
            result = std::wstring(buffer, charCount);
        }
    }
    
    return result;
}

bool UnicodeUtils::IsPrintableUnicode(const std::wstring& text) {
    if (text.empty()) return false;
    
    for (size_t i = 0; i < text.length(); ++i) {
        wchar_t ch = text[i];
        
        // Basic printable ASCII range
        if (ch >= 0x20 && ch <= 0x7E) {
            continue;
        }
        
        // Extended Latin and other basic ranges
        if (ch >= 0x80 && ch <= 0xFFFF) {
            // Check for various Unicode ranges that are printable
            if (ch >= 0x80 && ch <= 0x024F) continue;     // Latin Extended
            if (ch >= 0x1E00 && ch <= 0x1EFF) continue;   // Latin Extended Additional
            if (ch >= 0x2000 && ch <= 0x206F) continue;   // General Punctuation
            if (ch >= 0x20A0 && ch <= 0x20CF) continue;   // Currency Symbols
            if (ch >= 0x2100 && ch <= 0x214F) continue;   // Letterlike Symbols
            if (ch >= 0x2190 && ch <= 0x21FF) continue;   // Arrows
            if (ch >= 0x2200 && ch <= 0x22FF) continue;   // Mathematical Operators
            if (ch >= 0x2300 && ch <= 0x23FF) continue;   // Miscellaneous Technical
            if (ch >= 0x2460 && ch <= 0x24FF) continue;   // Enclosed Alphanumerics
            if (ch >= 0x25A0 && ch <= 0x25FF) continue;   // Geometric Shapes
            if (ch >= 0x2600 && ch <= 0x26FF) continue;   // Miscellaneous Symbols
            if (ch >= 0x2700 && ch <= 0x27BF) continue;   // Dingbats
        }
        
        // Handle surrogate pairs (for emojis and other high Unicode code points)
        if (IsHighSurrogate(ch) && i + 1 < text.length() && IsLowSurrogate(text[i + 1])) {
            // Calculate the actual code point from surrogate pair
            uint32_t highSurr = ch;
            uint32_t lowSurr = text[i + 1];
            uint32_t codePoint = 0x10000 + ((highSurr - 0xD800) << 10) + (lowSurr - 0xDC00);
            
            // Check if this is an emoji or symbol range
            if (IsEmojiCodePoint(codePoint)) {
                i++; // Skip the low surrogate since we processed the pair
                continue;
            }
        }
        
        // If we reach here, the character might not be printable
        // But be permissive for unknown ranges that might be valid
        if (ch >= 0x80) continue; // Allow most non-ASCII characters
        
        // Reject control characters
        if (ch < 0x20) return false;
    }
    
    return true;
}

bool UnicodeUtils::IsValidUnicodeSequence(const std::wstring& text) {
    if (text.empty()) return true;
    
    for (size_t i = 0; i < text.length(); ++i) {
        wchar_t ch = text[i];
        
        if (IsHighSurrogate(ch)) {
            // High surrogate must be followed by low surrogate
            if (i + 1 >= text.length() || !IsLowSurrogate(text[i + 1])) {
                return false; // Invalid surrogate pair
            }
            i++; // Skip the low surrogate
        } else if (IsLowSurrogate(ch)) {
            // Low surrogate without preceding high surrogate is invalid
            return false;
        }
    }
    
    return true;
}

size_t UnicodeUtils::GetUnicodeCharacterCount(const std::wstring& text) {
    size_t count = 0;
    for (size_t i = 0; i < text.length(); ++i) {
        if (IsHighSurrogate(text[i]) && i + 1 < text.length() && IsLowSurrogate(text[i + 1])) {
            // Surrogate pair counts as one character
            i++; // Skip the low surrogate
        }
        count++;
    }
    return count;
}

std::string UnicodeUtils::SafeWStringToUTF8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    
    // First validate the Unicode sequence
    if (!IsValidUnicodeSequence(wstr)) {
        // If invalid, try to clean it or return empty
        return std::string();
    }
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (size_needed <= 0) return std::string();
    
    std::string result(size_needed - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, NULL, NULL);
    
    return result;
}

std::wstring UnicodeUtils::SafeUTF8ToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (size_needed <= 0) return std::wstring();
    
    std::wstring result(size_needed - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);
    
    return result;
}

bool UnicodeUtils::IsHighSurrogate(wchar_t ch) {
    return (ch >= 0xD800 && ch <= 0xDBFF);
}

bool UnicodeUtils::IsLowSurrogate(wchar_t ch) {
    return (ch >= 0xDC00 && ch <= 0xDFFF);
}

bool UnicodeUtils::IsEmojiCodePoint(uint32_t codePoint) {
    // Check various emoji ranges
    if (codePoint >= 0x1F600 && codePoint <= 0x1F64F) return true; // Emoticons
    if (codePoint >= 0x1F300 && codePoint <= 0x1F5FF) return true; // Miscellaneous Symbols and Pictographs
    if (codePoint >= 0x1F680 && codePoint <= 0x1F6FF) return true; // Transport and Map Symbols
    if (codePoint >= 0x1F1E6 && codePoint <= 0x1F1FF) return true; // Regional Indicator Symbols (flags like 🇺🇸)
    if (codePoint >= 0x2600 && codePoint <= 0x26FF) return true;   // Miscellaneous Symbols
    if (codePoint >= 0x2700 && codePoint <= 0x27BF) return true;   // Dingbats
    if (codePoint >= 0x1F900 && codePoint <= 0x1F9FF) return true; // Supplemental Symbols and Pictographs
    if (codePoint >= 0x1FA70 && codePoint <= 0x1FAFF) return true; // Symbols and Pictographs Extended-A
    
    return false;
}