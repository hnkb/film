
#include "stdafx.h"
#include "image.h"


HBITMAP LoadHBitmap(const std::wstring filename)
{
	return GetHBitmap(ResizeWicBitmap(LoadWicBitmap(filename)));
}

CComPtr<IWICBitmapSource> LoadWicBitmap(const std::wstring filename)
{
	CComPtr<IWICImagingFactory>    pFactory;
	CComPtr<IWICBitmapDecoder>     pDecoder;
	CComPtr<IWICBitmapFrameDecode> pFrame;

	WICPixelFormatGUID pixelFormat = GUID_WICPixelFormatUndefined;

	HRESULT hr;

	hr = pFactory.CoCreateInstance(CLSID_WICImagingFactory);
	
	if (SUCCEEDED(hr)) hr = pFactory->CreateDecoderFromFilename(filename.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
	if (SUCCEEDED(hr)) hr = pDecoder->GetFrame(0, &pFrame);
	if (SUCCEEDED(hr)) hr = pFrame->GetPixelFormat(&pixelFormat);

	if (SUCCEEDED(hr))
	{
		if (pixelFormat == GUID_WICPixelFormat32bppBGRA) return CComQIPtr<IWICBitmapSource>(pFrame);
		
		CComPtr<IWICFormatConverter> pFmtCvt;
		BOOL bCanConvert = FALSE;

		hr = pFactory->CreateFormatConverter(&pFmtCvt);
		if (SUCCEEDED(hr)) hr = pFmtCvt->CanConvert(pixelFormat, GUID_WICPixelFormat32bppBGRA, &bCanConvert);
		if (SUCCEEDED(hr) && bCanConvert)
		{
			hr = pFmtCvt->Initialize(pFrame, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
			if (SUCCEEDED(hr)) return CComQIPtr<IWICBitmapSource>(pFmtCvt);
		}
	}

	return nullptr;
}

HBITMAP GetHBitmap(IWICBitmapSource* pSource)
{
	UINT width = 0;
	UINT height = 0;

	if (pSource == nullptr) return nullptr;
	if (FAILED(pSource->GetSize(&width, &height)) || width == 0 || height == 0) return nullptr;


	HBITMAP retval = nullptr;

	BITMAPINFO bminfo = {};
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biWidth = width;
	bminfo.bmiHeader.biHeight = -((LONG)height);
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;

	void* pvImageBits = nullptr;
	HDC hdcScreen = GetDC(nullptr);
	retval = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);
	ReleaseDC(nullptr, hdcScreen);

	if (retval)
	{
		const UINT cbStride = width * 4;
		const UINT cbImage = cbStride * height;

		if (FAILED(pSource->CopyPixels(nullptr, cbStride, cbImage, static_cast<BYTE*>(pvImageBits))))
		{
			DeleteObject(retval);
			retval = nullptr;
		}
	}

	return retval;
}

CComPtr<IWICBitmapSource> ResizeWicBitmap(IWICBitmapSource* pSource)
{
	CComPtr<IWICImagingFactory> pFactory;
	CComPtr<IWICBitmapScaler> pScaler;

	if (FAILED(pFactory.CoCreateInstance(CLSID_WICImagingFactory))) return nullptr;
	if (FAILED(pFactory->CreateBitmapScaler(&pScaler))) return nullptr;
	if (FAILED(pScaler->Initialize(pSource, 200, 300, WICBitmapInterpolationModeFant))) return nullptr;

	return CComQIPtr<IWICBitmapSource>(pScaler);
}
