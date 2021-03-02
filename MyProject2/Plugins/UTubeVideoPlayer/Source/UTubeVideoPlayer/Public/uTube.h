// Copyright 2020 Kelvin Rosa, All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Algo/Reverse.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Online/HTTP/Public/GenericPlatform/GenericPlatformHttp.h"
#include "Runtime/Core/Public/Internationalization/Regex.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "uTube.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegate_OnUrlLoaded, FString, _url);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegate_OnGetUrlError, FString, _error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDelegate_OnUrlLoadedMultiQuality, FString, _videoUrl, FString, _audioUrl);

UENUM()
enum Quality
{
	sd     UMETA(DisplayName = "SD(360p)"),
	hd      UMETA(DisplayName = "HD(720p)"),
	fullhd   UMETA(DisplayName = "FullHD"),
	none UMETA(DisplayName = "NONE"),
};

UCLASS()
class UTUBEVIDEOPLAYER_API AuTube : public AActor
{
	GENERATED_BODY()

public:
	FHttpModule* Http;
	FString youtubeUrl;

	AuTube();

	/* The actual HTTP call */
	void MyHttpCall(FString youtubeUrl);
	void GetJsForExtraction(FString url);

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseReceivedForJS(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);
	bool IsVideoUnavailable(FString pageSource);
	TArray<TSharedPtr<FJsonValue>> GetStreamMap(TSharedPtr<FJsonObject> jsonObject);
	TArray<TSharedPtr<FJsonValue>> GetAdaptiveStreamMap(TSharedPtr<FJsonObject> jsonObject);
	TMap<FString, FString> ParseQueryString(FString s);
	FString GetHtml5PlayerVersionForFix(FString _url);
	FString GetHtml5PlayerVersion(TSharedPtr<FJsonObject> jsonObject);
	void DecryptVideoUrl(FString actualUrl, FString videoUrl, FString htmlVersion);
	FString GetFunctionFromLine(FString currentLine);
	TArray<TCHAR> ReverseFunction(TArray<TCHAR> a);
	TArray<TCHAR> SpliceFunction(TArray<TCHAR> a, int32 b);
	TArray<TCHAR> SwapFunction(TArray<TCHAR> a, int32 b);



	UPROPERTY(BlueprintAssignable)
		FDelegate_OnUrlLoaded OnUrlLoaded;

	UPROPERTY(BlueprintAssignable)
		FDelegate_OnUrlLoadedMultiQuality OnUrlLoadedMultiQuality;

	UPROPERTY(BlueprintAssignable)
		FDelegate_OnGetUrlError OnGetUrlError;

	UFUNCTION(BlueprintCallable, Category = "Youtube")
		void LoadYoutubeUrl(FString youtubeUrl);

	UFUNCTION(BlueprintCallable, Category = "Youtube")
		void LoadYoutubeUrlMultiquality(FString youtubeUrl, TEnumAsByte<Quality> quality);

	
};

namespace ThreadingTest
{
	static FString GetFunctionFromLine(FString currentLine)
	{
		FString matchFunctionReg = "\\w+.\\(";
		const FRegexPattern myPattern(matchFunctionReg);
		FRegexMatcher myMatcher(myPattern, *currentLine);
		FString res;
		if (myMatcher.FindNext())
		{
			res = myMatcher.GetCaptureGroup(0);
			if (res.Contains("("))
			{
				int32 start = res.Find("(");
				res.RemoveAt(start);
			}
			UE_LOG(LogTemp, Warning, TEXT("V: %s"), *res);

		}
		return *res;
	}

	static TArray<TCHAR> ReverseFunction(TArray<TCHAR> a)
	{
		Algo::Reverse(a);
		return a;
	}

	static TArray<TCHAR> SpliceFunction(TArray<TCHAR> a, int32 index)
	{
		TArray<TCHAR> newArray;
		UE_LOG(LogTemp, Warning, TEXT("Total: %i Val: %i"), a.Num(), index);
		for (int32 idx = 0; idx < a.Num(); idx++)
		{
			if (a[idx] == '\0')
			{

			}
			else
			{
				newArray.Add(a[idx]);
			}
		}

		newArray.RemoveAt(0, index);

		return newArray;
	}

	static TArray<TCHAR> SwapFunction(TArray<TCHAR> a, int32 b)
	{
		if (a[0] == '\0')
		{
			a.RemoveAt(0);
		}
		TCHAR c = a[0];
		a[0] = a[b % a.Num()];
		a[b % a.Num()] = c;
		return a;
	}

	static TMap<FString, FString> ParseQueryString(FString s)
	{
		if (s.Contains("?"))
		{
			int32 start = s.Find("?");
			s.RemoveAt(0, start + 1);
			//UE_LOG(LogTemp, Warning, TEXT("ADDDAAAAP: %s"), *s);
		}
		TMap<FString, FString> dictionary;
		TArray<FString> firstSplit;
		s.ParseIntoArray(firstSplit, TEXT("&"), false);

		for (FString firstSplited : firstSplit)
		{
			TArray<FString> secondSplit;
			firstSplited.ParseIntoArray(secondSplit, TEXT("="), false);
			if (secondSplit.Num() == 2)
			{
				FString unscapedUrl = FGenericPlatformHttp::UrlDecode(secondSplit[1]);
				dictionary.Add(secondSplit[0], unscapedUrl);
			}
			else if (secondSplit.Num() > 2) //for multiple = simbols in same content.
			{
				int32 c = 0;
				FString correct = "";
				for (FString sec : secondSplit)
				{
					if (c > 0)
					{
						if (c == 1)
							correct += sec;
						else
							correct += "=" + sec;
					}
					c++;
				}
				FString unscapedUrl = FGenericPlatformHttp::UrlDecode(correct);
				dictionary.Add(secondSplit[0], unscapedUrl);
			}
			else
			{
				dictionary.Add(secondSplit[0], "");
			}
		}

		return dictionary;
	}


	//Did this function to work async with multithread
	static void DecryptVideoUrlx(FString resultJS, FString sig, FString lsig, FString encryptedUrl, FString videoSig, FString videoLSig, FString videoEncryptedUrl, AuTube *service)
	{
		
		FString js = resultJS;
		UE_LOG(LogTemp, Warning, TEXT("TEST"));
		//FString functNamePattern = "\\b[cs]\\s*&&\\s*[adf]\\.set\\([^,]+\\s*,\\s*encodeURIComponent\\s*\\(\\s*([\\w$]+)\\(";
		FString functNamePattern = "(?:\\b|[^\\w$])([\\w$]\\2)\\s*=\\s*function\\(\\s*a\\s*\\)\\s*{\\s*a\\s*=\\s*a\\.split\\(\\s*""""\\s*\\)";
		const FRegexPattern myPatternForJs(functNamePattern);
		FRegexMatcher myMatcher(myPatternForJs, *js);
		FString funcName;

		if (myMatcher.FindNext())
		{
			UE_LOG(LogTemp, Warning, TEXT("SS"));
			funcName = myMatcher.GetCaptureGroup(1);
			UE_LOG(LogTemp, Warning, TEXT("FUNCNAME IS: %s"), *funcName);

		}
		UE_LOG(LogTemp, Warning, TEXT("haha: %s"), *videoEncryptedUrl);

		FString funcPattern = "(?!h\\.)" + funcName + "=function\\(\\w+\\)\\{.*?\\}";

		FString funcBody;

		const FRegexPattern myPatternForBody2(funcPattern);
		FRegexMatcher myMatcherBody2(myPatternForBody2, *js);
		TArray<FString> lines;
		FString idReverse = "", idSlice = "", idCharSwap = "";
		FString functionIdentifier = "";
		FString operations = "";

		TArray<TCHAR> sigArray = sig.GetCharArray();


		if (myMatcherBody2.FindNext())
		{
			funcBody = myMatcherBody2.GetCaptureGroup(0);
			UE_LOG(LogTemp, Warning, TEXT("FUNCBODY IS: %s"), *funcBody);

			funcBody.ParseIntoArray(lines, TEXT(";"), false);


			UE_LOG(LogTemp, Warning, TEXT("LINES: %i"), lines.Num());
			int32 index = 0;
			for (FString line : lines)
			{
				if (index != 0 && index != (lines.Num() - 1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Line: %s"), *line);
					if (idReverse != "" && idSlice != "" && idCharSwap != "")
					{
						break;
					}

					functionIdentifier = GetFunctionFromLine(*line);
					FString reReversePattern = functionIdentifier + ":\\bfunction\\b\\(\\w+\\)"; //Regex for reverse (one parameter)
					FString reSlicePattern = functionIdentifier + ":\\bfunction\\b\\([a],b\\).(\\breturn\\b)?.?\\w+\\."; //Regex for slice (return or not)
					FString reSwapPattern = functionIdentifier + ":\\bfunction\\b\\(\\w+\\,\\w\\).\\bvar\\b.\\bc=a\\b"; //Regex for the char swap.
					FString fTest = "encodeURIComponent:\\bfunction\\b\\(\\w+\\)";//TODO IMPROVE THIS.

					const FRegexPattern reReverse(reReversePattern);
					FRegexMatcher reReverseMatch(reReverse, *js);
					const FRegexPattern reSlice(reSlicePattern);
					FRegexMatcher reSliceMatch(reSlice, *js);
					const FRegexPattern reSwap(reSwapPattern);
					FRegexMatcher reSwapMatch(reSwap, *js);
					const FRegexPattern failTest(fTest);//TODO IMPROVE THIS.
					FRegexMatcher failTestMatch(failTest, *js);//TODO IMPROVE THIS.

					if (reReverseMatch.FindNext())
					{
						if (functionIdentifier == "nt") 
						{
							
								if (failTestMatch.FindNext())	//TODO IMPROVE THIS.
								{
									//if found uri component dont use it igore;
								}
								else
								{
									idReverse = functionIdentifier; 
								}
						}
						else 
						{
							idReverse = functionIdentifier;
						}
						
					}

					if (reSliceMatch.FindNext())
					{
						idSlice = functionIdentifier;
					}

					if (reSwapMatch.FindNext())
					{
						idCharSwap = functionIdentifier;
					}
				}
				index++;
			}

			UE_LOG(LogTemp, Warning, TEXT("Reverse: %s"), *idReverse);
			UE_LOG(LogTemp, Warning, TEXT("Swap: %s"), *idCharSwap);
			UE_LOG(LogTemp, Warning, TEXT("Slice: %s"), *idSlice);
			UE_LOG(LogTemp, Warning, TEXT("Lines: %i"), lines.Num());

			if (sigArray.Num() > 0)
			{
				index = 0;
				for (FString line : lines)
				{
					
						if (index != 0 && index != (lines.Num() - 1))
						{
							UE_LOG(LogTemp, Warning, TEXT("!!!Line: %s"), *line);
							functionIdentifier = GetFunctionFromLine(*line);

							if (functionIdentifier == idCharSwap)
							{
								TArray<FString> swapSplit;
								line.ParseIntoArray(swapSplit, TEXT(","), false);
								int32 n = swapSplit[1].Find(")");
								swapSplit[1].RemoveAt(n);
								UE_LOG(LogTemp, Warning, TEXT("SWAAAP %s"), *swapSplit[1]);
								int32 op = FCString::Atoi(*swapSplit[1]);
								operations += "w" + swapSplit[1] + " ";
								FString temp;
								for (int i = 0; i < sigArray.Num(); i++) {
									temp.AppendChar(sigArray[i]);
								}
								UE_LOG(LogTemp, Warning, TEXT("SW %s"), *temp);
								sigArray = SwapFunction(sigArray, op);
								temp = "";
								for (int i = 0; i < sigArray.Num(); i++) {
									temp.AppendChar(sigArray[i]);
								}
								UE_LOG(LogTemp, Warning, TEXT("SWR %s"), *temp);
							}

							if (functionIdentifier == idSlice)
							{
								TArray<FString> sliceSplit;
								line.ParseIntoArray(sliceSplit, TEXT(","), false);
								int32 n = sliceSplit[1].Find(")");
								sliceSplit[1].RemoveAt(n);
								int32 op = FCString::Atoi(*sliceSplit[1]);
								UE_LOG(LogTemp, Warning, TEXT("SLIIICE %s"), *sliceSplit[1]);
								operations += "s" + sliceSplit[1] + " ";
								FString temp;
								for (int i = 0; i < sigArray.Num(); i++) {
									temp.AppendChar(sigArray[i]);
								}
								UE_LOG(LogTemp, Warning, TEXT("S %s"), *temp);

								sigArray = SpliceFunction(sigArray, op);

								temp = "";
								for (int i = 0; i < sigArray.Num(); i++) {
									temp.AppendChar(sigArray[i]);
								}
								UE_LOG(LogTemp, Warning, TEXT("SR %s"), *temp);
							}

							if (functionIdentifier == idReverse)
							{
								UE_LOG(LogTemp, Warning, TEXT("REVERSE!"));
								operations += "r ";
								FString temp;
								for (int i = 0; i < sigArray.Num(); i++) {
									temp.AppendChar(sigArray[i]);
								}
								UE_LOG(LogTemp, Warning, TEXT("R %s"), *temp);
								sigArray = ReverseFunction(sigArray);

								temp = "";
								for (int i = 0; i < sigArray.Num(); i++) {
									temp.AppendChar(sigArray[i]);
								}
								UE_LOG(LogTemp, Warning, TEXT("RR %s"), *temp);
							}

						}
					

					index++;
				}
				operations = operations.TrimStart();
				UE_LOG(LogTemp, Warning, TEXT("Operation: %s"), *operations);

				FString s;
				for (int i = 0; i < sigArray.Num(); i++) {
					s.AppendChar(sigArray[i]);
				}

				UE_LOG(LogTemp, Warning, TEXT("Signature: %s"), *s);

				UE_LOG(LogTemp, Warning, TEXT("Signature: %s"), *encryptedUrl);

				TMap<FString, FString> qq = ParseQueryString(encryptedUrl);
				qq["sig"] = s;
				UE_LOG(LogTemp, Warning, TEXT("hahax: %s"), *videoEncryptedUrl);
				FString resultString;

				TArray<FString> mainUrlSplited;
				encryptedUrl.ParseIntoArray(mainUrlSplited, TEXT("?"), false);
				FString mainUrl = mainUrlSplited[0];

				bool isFirst = true;
				for (TPair<FString, FString> pair : qq)
				{
					if (!isFirst)
					{
						resultString.Append("&");
					}

					if (pair.Key == "lsig") {
						if (pair.Value == "")
						{
							resultString.Append(pair.Key);
							resultString.Append("=");
							resultString.Append(lsig);
						}
						else
						{
							resultString.Append(pair.Key);
							resultString.Append("=");
							resultString.Append(pair.Value);
						}
					}
					else {
						resultString.Append(pair.Key);
						resultString.Append("=");
						resultString.Append(pair.Value);
					}

					//UE_LOG(LogTemp, Warning, TEXT("r: %s"), *resultString);
					isFirst = false;
				}

				//audio or sdqual
				FString finalUrl = mainUrl + "?" + resultString;
				//-------------------------------------------------------VIDEO START--------------------------
				UE_LOG(LogTemp, Warning, TEXT("now: %s"), *videoEncryptedUrl);
				if(videoEncryptedUrl != "")
				{
					UE_LOG(LogTemp, Warning, TEXT("K: %s"), *videoEncryptedUrl);
					sigArray = videoSig.GetCharArray();
					const FRegexPattern myPatternForBody(funcPattern);
					FRegexMatcher myMatcherBody(myPatternForBody, *js);
					if (myMatcherBody.FindNext())
					{
						funcBody = myMatcherBody.GetCaptureGroup(0);
						UE_LOG(LogTemp, Warning, TEXT("FUNCBODY IS: %s"), *funcBody);

						funcBody.ParseIntoArray(lines, TEXT(";"), false);


						UE_LOG(LogTemp, Warning, TEXT("LINES: %i"), lines.Num());
						index = 0;
						for (FString line : lines)
						{
							if (index != 0 && index != (lines.Num() - 1))
							{
								UE_LOG(LogTemp, Warning, TEXT("Line: %s"), *line);
								if (idReverse != "" && idSlice != "" && idCharSwap != "")
								{
									break;
								}

								functionIdentifier = GetFunctionFromLine(*line);
								FString reReversePattern = functionIdentifier + ":\\bfunction\\b\\(\\w+\\)"; //Regex for reverse (one parameter)
								FString reSlicePattern = functionIdentifier + ":\\bfunction\\b\\([a],b\\).(\\breturn\\b)?.?\\w+\\."; //Regex for slice (return or not)
								FString reSwapPattern = functionIdentifier + ":\\bfunction\\b\\(\\w+\\,\\w\\).\\bvar\\b.\\bc=a\\b"; //Regex for the char swap.
								FString fTest = "encodeURIComponent:\\bfunction\\b\\(\\w+\\)";//TODO IMPROVE THIS.

								const FRegexPattern reReverse(reReversePattern);
								FRegexMatcher reReverseMatch(reReverse, *js);
								const FRegexPattern reSlice(reSlicePattern);
								FRegexMatcher reSliceMatch(reSlice, *js);
								const FRegexPattern reSwap(reSwapPattern);
								FRegexMatcher reSwapMatch(reSwap, *js);

								const FRegexPattern failTest(fTest);//TODO IMPROVE THIS.
								FRegexMatcher failTestMatch(failTest, *js);//TODO IMPROVE THIS.

								if (reReverseMatch.FindNext())
								{
									if (functionIdentifier == "nt")
									{

										if (failTestMatch.FindNext())	//TODO IMPROVE THIS.
										{
											//if found uri component dont use it igore;
										}
										else
										{
											idReverse = functionIdentifier;
										}
									}
									else
									{
										idReverse = functionIdentifier;
									}

								}

								if (reSliceMatch.FindNext())
								{
									idSlice = functionIdentifier;
								}

								if (reSwapMatch.FindNext())
								{
									idCharSwap = functionIdentifier;
								}
							}
							index++;
						}

						UE_LOG(LogTemp, Warning, TEXT("Reverse: %s"), *idReverse);
						UE_LOG(LogTemp, Warning, TEXT("Swap: %s"), *idCharSwap);
						UE_LOG(LogTemp, Warning, TEXT("Slice: %s"), *idSlice);
						UE_LOG(LogTemp, Warning, TEXT("Lines: %i"), lines.Num());

						if (sigArray.Num() > 0)
						{
							index = 0;
							for (FString line : lines)
							{

								if (index != 0 && index != (lines.Num() - 1))
								{
									UE_LOG(LogTemp, Warning, TEXT("!!!Line: %s"), *line);
									functionIdentifier = GetFunctionFromLine(*line);

									if (functionIdentifier == idCharSwap)
									{
										TArray<FString> swapSplit;
										line.ParseIntoArray(swapSplit, TEXT(","), false);
										int32 n = swapSplit[1].Find(")");
										swapSplit[1].RemoveAt(n);
										UE_LOG(LogTemp, Warning, TEXT("SWAAAP %s"), *swapSplit[1]);
										int32 op = FCString::Atoi(*swapSplit[1]);
										operations += "w" + swapSplit[1] + " ";
										FString temp;
										for (int i = 0; i < sigArray.Num(); i++) {
											temp.AppendChar(sigArray[i]);
										}
										UE_LOG(LogTemp, Warning, TEXT("SW %s"), *temp);
										sigArray = SwapFunction(sigArray, op);
										temp = "";
										for (int i = 0; i < sigArray.Num(); i++) {
											temp.AppendChar(sigArray[i]);
										}
										UE_LOG(LogTemp, Warning, TEXT("SWR %s"), *temp);
									}

									if (functionIdentifier == idSlice)
									{
										TArray<FString> sliceSplit;
										line.ParseIntoArray(sliceSplit, TEXT(","), false);
										int32 n = sliceSplit[1].Find(")");
										sliceSplit[1].RemoveAt(n);
										int32 op = FCString::Atoi(*sliceSplit[1]);
										UE_LOG(LogTemp, Warning, TEXT("SLIIICE %s"), *sliceSplit[1]);
										operations += "s" + sliceSplit[1] + " ";
										FString temp;
										for (int i = 0; i < sigArray.Num(); i++) {
											temp.AppendChar(sigArray[i]);
										}
										UE_LOG(LogTemp, Warning, TEXT("S %s"), *temp);

										sigArray = SpliceFunction(sigArray, op);

										temp = "";
										for (int i = 0; i < sigArray.Num(); i++) {
											temp.AppendChar(sigArray[i]);
										}
										UE_LOG(LogTemp, Warning, TEXT("SR %s"), *temp);
									}

									if (functionIdentifier == idReverse)
									{
										UE_LOG(LogTemp, Warning, TEXT("REVERSE!"));
										operations += "r ";
										FString temp;
										for (int i = 0; i < sigArray.Num(); i++) {
											temp.AppendChar(sigArray[i]);
										}
										UE_LOG(LogTemp, Warning, TEXT("R %s"), *temp);
										sigArray = ReverseFunction(sigArray);

										temp = "";
										for (int i = 0; i < sigArray.Num(); i++) {
											temp.AppendChar(sigArray[i]);
										}
										UE_LOG(LogTemp, Warning, TEXT("RR %s"), *temp);
									}

								}


								index++;
							}
							operations = operations.TrimStart();
							UE_LOG(LogTemp, Warning, TEXT("Operation: %s"), *operations);

							FString s2;
							for (int i = 0; i < sigArray.Num(); i++) {
								s2.AppendChar(sigArray[i]);
							}

							UE_LOG(LogTemp, Warning, TEXT("Signature: %s"), *s2);

							UE_LOG(LogTemp, Warning, TEXT("Signature: %s"), *videoEncryptedUrl);

							TMap<FString, FString> qq2 = ParseQueryString(videoEncryptedUrl);
							qq2["sig"] = s2;

							FString resultString2;

							TArray<FString> mainUrlSplited2;
							videoEncryptedUrl.ParseIntoArray(mainUrlSplited2, TEXT("?"), false);
							FString mainUrl2 = mainUrlSplited2[0];

							bool isFirst2 = true;
							for (TPair<FString, FString> pair : qq2)
							{
								if (!isFirst2)
								{
									resultString2.Append("&");
								}

								if (pair.Key == "lsig") {
									if (pair.Value == "")
									{
										resultString2.Append(pair.Key);
										resultString2.Append("=");
										resultString2.Append(videoLSig);
									}
									else
									{
										resultString2.Append(pair.Key);
										resultString2.Append("=");
										resultString2.Append(pair.Value);
									}
								}
								else {
									resultString2.Append(pair.Key);
									resultString2.Append("=");
									resultString2.Append(pair.Value);
								}

								//UE_LOG(LogTemp, Warning, TEXT("r: %s"), *resultString);
								isFirst2 = false;
							}

							//audio or sdqual
							FString finalUrlVideo = mainUrl2 + "?" + resultString2;
							UE_LOG(LogTemp, Warning, TEXT("r: %s"), *finalUrlVideo);
							UE_LOG(LogTemp, Warning, TEXT("ra: %s"), *finalUrl);

							if (videoEncryptedUrl != "")
							{
								service->OnUrlLoadedMultiQuality.Broadcast(finalUrlVideo, finalUrl);
							}
						}
						else {
							service->OnGetUrlError.Broadcast("Empty signature try again");
						}
					}
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Final Url: %s"), *finalUrl);
					service->OnUrlLoaded.Broadcast(finalUrl);
				}

			}
			else {
				service->OnGetUrlError.Broadcast("Empty signature try again");
			}
		}
	}
}

class VideoDecryptionAsync : public FNonAbandonableTask
{
	FString resultJS;
	FString sig;
	FString lsig;
	FString encryptedUrl;
	FString videoSig;
	FString videoLSig;
	FString videoEncryptedUrl;
	AuTube *service;

public:
	/*Default constructor*/
	VideoDecryptionAsync(FString resultJS, FString sig, FString lsig, FString encryptedUrl, FString videoSig, FString videoLSig, FString videoEncryptedUrl, AuTube *service)
	{
		this->resultJS = resultJS;
		this->sig = sig;
		this->lsig = lsig;
		this->encryptedUrl = encryptedUrl;
		this->videoSig = videoSig;
		this->videoLSig = videoLSig;
		this->videoEncryptedUrl = videoEncryptedUrl;
		this->service = service;
	}

	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(VideoDecryptionAsync, STATGROUP_ThreadPoolAsyncTasks);
	}

	/*This function is executed when we tell our task to execute*/
	void DoWork()
	{
		ThreadingTest::DecryptVideoUrlx(resultJS, sig, lsig, encryptedUrl, videoSig, videoLSig, videoEncryptedUrl, service);
		//clear to dont have acces anymore and throw errors.
		service = nullptr;
		GLog->Log("--------------------------------------------------------------------");
		GLog->Log("Youtube url is decrypted, ready to pass to the player");
		GLog->Log("--------------------------------------------------------------------");
	}
};
