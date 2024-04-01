#include "strings.hpp"

std::string Version = "VidInjector9000 ver. 3.0.5-CLI";
std::string LogoASCII = "          @@@@@@@@@\n"
						"        @@@@@@@@@@@@@\n"
						"     @@@@@@@   @@@@@@@\n"
						"  @@@@@@@@@@@@@@@@@@@@@@\n"
						"@@@@@@@@@@@@@@@@@@@@@@@@@\n"
						"@@@@@@@@@@@@   @@@@@@@@@@@\n"
						"@@@@@@@@@@@@   @@@@@@@@@@@@\n"
						"@@@@@@@@@@@@   @@@@@@@@@@@@\n"
						" @@@@@@@@@@@   @@@@@@@@@@@@\n"
						"  @@@@@@@@@@   @@@@@@@@@@@@\n"
						"   @@@@@@@@@   @@@@@@@@@@\n"
						"     @@@@@@@   @@@@@@@@\n"
						"      @@@@@@@@@@@@@@\n"
						"        @@@@@@@@@\n";
std::string HArg = "-help";
std::string HArgShort = "-h";
std::string NArg = "-new";
std::string NArgShort = "-n";
std::string SpArg = "-set_parameter";
std::string SpArgShort = "-sp";
std::string PpArg = "-print_parameters";
std::string PpArgShort = "-pp";
std::string ArArg = "-add_row";
std::string ArArgShort = "-ar";
std::string SrArg = "-subtract_row";
std::string SrArgShort = "-sr";
std::string BcArg = "-build_cia";
std::string BcArgShort = "-bc";
std::string EcArg = "-extract_cia";
std::string EcArgShort = "-ec";
std::string EtArg = "-extract_tar";
std::string EtArgShort = "-et";

std::string Languagedir = "English";

std::string resourcesPath = "Vidinjector9000Resources";
std::string VI9PVER = "VI9P100";
std::string StrVerParam = "STR:VER";
std::string IntMultiParam = "INT:MULTI";
std::string StrBannerParam = "STR:BANNER";
std::string StrIconParam = "STR:ICON";
std::string IntIconBorderParam = "INT:ICONBORDER";
std::string StrSNameParam = "STR:SNAME";
std::string StrLNameParam = "STR:LNAME";
std::string StrPublisherParam = "STR:PUBLISHER";
std::string IntCopycheckParam = "INT:COPYCHECK";
std::string StrCopyrightParam = "STR:COPYRIGHT";
std::string IntFFrewindParam = "INT:FFREWIND";
std::string IntFadeOptParam = "INT:FADEOPT";
std::string IntRowsParam = "INT:ROWS";
std::string StrPTitleParam = "STR:PTITLE";
std::string StrMoflexParam = "STR:MOFLEX";
std::string StrMBannerParam = "STR:MBANNER";
std::string IntSplitPatchParam = "INT:SPLITPATCH";
std::string IntPreIndexParam = "INT:PREINDEX";

std::string ProgramName = "VidInjector9002 CLI Edition";
std::string ByMeText = "by FoofooTheGuy";
std::string InfoText = "A console program that interfaces with a VidInjector9002 parameters file.";
std::string UsageText = "Usage:";
std::string ExampleText = "<argument> [parameters...]";
std::string ArgumentText = "Arguments:";
//std::string ParametersText = "Parameter(s):";
//std::string InformationText = "Information:";
//std::string HelpInfoText = "The obligatory display-info-about-how-to-use-the-program argument";
//std::string Help2InfoText = "Display information about an argument mentioned in the `-help` argument list";
std::string NewInfoText = "Create a .vi9p file with default parameters.";
std::string SetPInfoText = "Set a parameter based on which number of parameter to set. (Output file can be the same as input file)";
std::string PrintPInfoText = "Print the parameters in a .vi9p file in a numbered list.";
std::string AddRInfoText = "Add parameters for a row and increment the INT:ROWS parameter. (Output file can be the same as input file)";
std::string SubRInfoText = "Remove the parameters from the last row and decrement the INT:ROWS parameter. (Output file can be the same as input file)";
std::string BuildCInfoText = "Build a .cia from parameters file. Note that the title IDs for this must range from C0000 - EFFFF in order to avoid confict with other titles.";
std::string BuildC2InfoText = "The same as the first one, except with the following defaults where the argument parameters are missing: (random unique ID) 'video' 'VDIJ'";
std::string BuildC3InfoText = "The same as the previous one, except with an output .tar which will contain a luma game patch. (Only use this if INT:SPLITPATCH is set in the input .vi9p)";
std::string ExCInfoText = "Extract parameters from a .cia to a directory which will contain the romfs directory, exefs directory, and .vi9p file. Note that unnecessary files will not be extracted.";
std::string ExC2InfoText = "The same as the previous one, except with the seed to decrypt the content.";
std::string EtInfoText = "Extract patch parameters from a .tar to a directory which will contain the romfs directory and .vi9p file. Set <output directory> to the directory of an extracted .cia to combine it all.";
std::string InVi9pFile = "input .vi9p file";
std::string InCiaFile = "input .cia file";
std::string InTarFile = "input .tar file";
std::string OutVi9pFile = "output .vi9p file";
std::string OutCiaFile = "output .cia file";
std::string OutTarFile = "output .tar file";
std::string OutDir = "output directory";
std::string SeedFile = "seed file";
std::string SeedNotFound = "Seed not found.";
std::string number = "number";
std::string NewValueText = "new value";
std::string UniqueIDText = "unique ID";
std::string AppTitleText = "application title";
std::string ProdCodeLatt = "product code latter";
std::string OutDirText = "output directory";
std::string ErrorText = "Error:";
std::string UnknownArgText = "Unknown argument:";
std::string BadValue = "Bad value";
std::string BadVersion = "Bad version";
std::string SupportedVersion = "Supported version:";
std::string BeANumber = "must be a number";
std::string MissingVariableError = "Missing variable";
std::string FailedToFindVar = "Failed to find the variable";
std::string FailedToFindPath = "Failed to find the path";
std::string FailedToFindFile = "Failed to find the file";
std::string FailedToReadFile = "Failed to read the file";
std::string FailedToCopyFile = "Failed to copy the file";
std::string FailedToCreateFile = "Failed to create the file:";
std::string FailedToConvertImage = "Failed to convert the image:";
std::string FailedToReadExHeader = "Failed to read ExHeader from:";
std::string ValueNoChange = "The value will not be changed";
std::string noMoreThan27 = "There cannot be more than 27 rows";
std::string ParametersLoaded = "Parameters loaded";
std::string SuccessfullyLoaded = "Successfully loaded";
std::string FailedToLoad = "Failed to load some values from";
std::string ValidStillLoaded = "(Any valid data was still loaded)";
std::string CreatingFile = "Creating the file:";
std::string CopyingMoflex = "Copying MoFlex files...";
std::string MoflexError = "is not in MoFlex format";
std::string ProductCodetext = "Product code:";
