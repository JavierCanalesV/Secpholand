// Copyright 2017-2020 HowToCompute. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class NetShield : ModuleRules
{
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/")); }
    }

    public NetShield(ReadOnlyTargetRules Target) : base(Target)
    {
        // Force IWYU
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Load the OpenSSL library. Though previously handled by LoadOpenSSL, now using the OpenSSL module to avoid any symbol duplication errors with source builds/while packaging.
        PublicDependencyModuleNames.Add("OpenSSL");
        
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
		

        if (Target.bBuildEditor == true)
        {
            System.Console.WriteLine("Building NetShield for the Editor! Including PIE bypass...");
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "UnrealEd"
                });
        }
    }

    public bool LoadOpenSSL(ReadOnlyTargetRules Target)
    {
        bool libSupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64))
        {
            libSupported = true;

            string LibrariesPath = Path.Combine(ThirdPartyPath, "OpenSSL", "lib", "Win64");

            // Add the libssl and libcrypto windows library
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libssl.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libcrypto.lib"));

            // Include the win64 specific headers
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "OpenSSL", "incw64"));
        }
        else if ((Target.Platform == UnrealTargetPlatform.Android))   
        {
            libSupported = true;

            string LibrariesPath = Path.Combine(ThirdPartyPath, "OpenSSL", "lib", "Android");

            // Add the libssl and libcrypto android library
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libssl.a"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libcrypto.a"));

            // Include the android specific headers
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "OpenSSL", "inca"));
        }
        // Not officially supported!
        else if ((Target.Platform == UnrealTargetPlatform.Linux))
        {
            libSupported = true;

            string LibrariesPath = Path.Combine(ThirdPartyPath, "OpenSSL", "lib", "Linux64");

            // Add the libssl and libcrypto linux library
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libssl.a"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libcrypto.a"));

            // Include the linux64 specific headers
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "OpenSSL", "incl64"));
        }
        else
        {
            System.Console.WriteLine("The platform your trying to build for is currently unsupported by the NetShield plugin!");
        }

        if (libSupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "OpenSSL", "inc"));
        }

        return libSupported;
    }
}
