
#pragma once

#include <string>
#include <Windows.h>
#include <atlbase.h>
#include <wincodec.h>

HBITMAP LoadHBitmap(const std::wstring filename);
CComPtr<IWICBitmapSource> LoadWicBitmap(const std::wstring filename);
HBITMAP GetHBitmap(IWICBitmapSource* pSource);
CComPtr<IWICBitmapSource> ResizeWicBitmap(IWICBitmapSource* pSource);
