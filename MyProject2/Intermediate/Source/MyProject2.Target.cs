using UnrealBuildTool;

public class MyProject2Target : TargetRules
{
	public MyProject2Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("MyProject2");
	}
}
