using UnrealBuildTool;

public class DbPlusEditor : ModuleRules
{
	public DbPlusEditor(ReadOnlyTargetRules Target) 
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

		PrivateDependencyModuleNames.AddRange(
			new string[] {
              	"AppFramework",
                "AssetRegistry",
                "BlueprintGraph",
                "ClassViewer",
                "Core",
				"CoreUObject",
                "Documentation",
                "EditorStyle",
                "EditorWidgets",
                "Engine",
                "GraphEditor",
                "InputCore",
                "Kismet",
                "KismetCompiler",
                "KismetWidgets",
                "PropertyEditor",
                "RenderCore",
                "RHI",
                "Slate",
				"SlateCore",
				"UnrealEd",
			});

        //CircularlyReferencedDependentModules.AddRange(
        //    new string[] {
        //        "KismetCompiler",
        //        "UnrealEd",
        //        "GraphEditor",
        //        "Kismet",
        //    });

        DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"ContentBrowser",
			});
	}
}
