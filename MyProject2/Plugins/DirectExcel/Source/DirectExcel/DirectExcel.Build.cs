// Copyright 2018 Jianzhao Fu. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class DirectExcel : ModuleRules
{
	public DirectExcel(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
                "Projects",
				"XlntLib"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		

        PublicDefinitions.Add("XLNT_STATIC=1");
        //bEnableExceptions = true;

	}
}
