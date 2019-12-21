using UnrealBuildTool;

public class DbPlus : ModuleRules
{
	public DbPlus(ReadOnlyTargetRules Target) 
		: base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		if (Target.Version.MinorVersion <= 19)
	    {
			PublicIncludePaths.AddRange(
				new string[] {
				});

			PrivateIncludePaths.AddRange(
				new string[] {
				});
		}

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
                "DatabaseSupport",

                "AwesomeService",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
                "DatabaseSupport",
				"Engine",
				"Slate",
				"SlateCore",

                "AwesomeService",
			});

        if (Target.bBuildEditor == true)
	        PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
	}
}
