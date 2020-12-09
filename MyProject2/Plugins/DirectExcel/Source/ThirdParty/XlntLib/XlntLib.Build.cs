// Copyright 2018 Jianzhao Fu. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class XlntLib : ModuleRules
{
    public XlntLib(ReadOnlyTargetRules Target):base(Target)
    {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
	    bFasterWithoutUnity = true;
		PublicIncludePaths.Add(ModuleDirectory);
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "xlnt"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "utfcpp"));


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"SlateCore"
			    // ... add other public dependencies that you statically link with here ...
		    }
		);


	    PrivateDependencyModuleNames.AddRange(
		    new string[]
		    {
			    "CoreUObject",
			    "Engine",
		    });

		PublicDefinitions.Add("XLNT_STATIC=1");
		//bEnableExceptions = true;
	}
}
