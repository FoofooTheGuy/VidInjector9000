#include "strings.hpp"

std::string Version = "VidInjector9000 ver. 3.5.1-CLI";
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
std::string SsArg = "-set_smdh";
std::string SsArgShort = "-ss";
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
std::string GpArg = "-generate_preview";
std::string GpArgShort = "-gp";
std::string RrArg = "-return_rows";
std::string RrArgShort = "-rr";

std::string Languagedir = "English";

std::string resourcesPath = "VidInjector9000Resources";
std::string VI9PVER = "VI9P101";
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
//std::string IntPreIndexParam = "INT:PREINDEX";

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
std::string SpInfoText = "Set a parameter based on which number of parameter to set. (Do -pp to see a list of possible numbers) (Output file can be the same as input file)";
std::string SsInfoText = "Similar to -sp, this reads the titles from an SMDH and sets them to short name, long name, and publisher in the output .vi9p file. Useful for when first using an SMDH for the icon (Do -pp to see a list of possible numbers) (Output file can be the same as input file)";
std::string PpInfoText = "Print the parameters in a .vi9p file in a numbered list.";
std::string ArInfoText = "Add parameters for a row and increment the INT:ROWS parameter. (Output file can be the same as input file)";
std::string SrInfoText = "Remove the parameters from the last row and decrement the INT:ROWS parameter. (Output file can be the same as input file)";
std::string BcInfoText = "Build a .cia from parameters file. Note that the title IDs for this must range from C0000 - EFFFF in order to avoid confict with other titles.";
std::string Bc2InfoText = "The same as the first one, except with the following defaults where the argument parameters are missing: (random unique ID) 'video' 'VDIJ'";
std::string Bc3InfoText = "The same as the previous one, except with an output .tar which will contain a Luma LayeredFS game patch. (Only use this if INT:SPLITPATCH is set in the input .vi9p)";
std::string EcInfoText = "Extract parameters from a .cia to a directory which will contain the romfs directory, exefs directory, and .vi9p file. Note that unnecessary files will not be extracted.";
std::string Ec2InfoText = "The same as the previous one, except with the seed to decrypt the content.";
std::string EtInfoText = "Extract patch parameters from a .tar to a directory which will contain the romfs directory and .vi9p file. Set <output directory> to the directory of an extracted .cia to combine it all.";
std::string GpInfoText = "Generate a preview image for the banner or icon that matches <number>. (Do -pp to see a list of possible numbers)";
std::string RrInfoText = "Return the value of INT:ROWS from <input .vi9p file>. Note: This is only for reading purposes. Use -ar or -sr to change the amount of rows.";
std::string InVi9pFile = "input .vi9p file";
std::string InCiaFile = "input .cia file";
std::string InTarFile = "input .tar file";
std::string OutVi9pFile = "output .vi9p file";
std::string OutCiaFile = "output .cia file";
std::string OutTarFile = "output .tar file";
std::string OutPngFile = "output .png file";
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
std::string NothingToDo = "Nothing to do.";
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
std::string CIAopenInput = "Open the input file...";
std::string CIAopenOutput = "Open the output file...";
std::string CIAreadHeader = "Read the CIA header...";
std::string CIAopenCertificate = "Open the certificate chain...";
std::string CIAopenTicket = "Open the ticket...";
std::string CIAopenTMD = "Open the TMD...";
std::string CIAparseTicket = "Parse the ticket...";
std::string CIAcreateReader = "Create a content reader...";
std::string CIAopenFirstContent = "Open the first content...";
std::string CIAparseNCCH = "Parse the NCCH header...";
std::string CIAinitializeVFS = "Initialize a VFS...";
std::string CIApopulateVFS = "Populate the VFS...";
std::string CIAscanSeeddb = "Scan for a SeedDB...";
std::string CIAgenerateKeys = "Generate the cryptographic keys...";
std::string CIAopenExheader = "Open the exheader file...";
std::string CIAreadExheader = "Read the exheader...";
std::string CIAopenLogo = "Open logo stream...";
std::string CIAsetParameters = "Set up the parameters for building...";
std::string CIAsetTitleID = "Set title ID...";
std::string CIAwriteCIA = "Write CIA...";
std::string CIAcleanUp = "Cleaning up...";
std::string TARremoveExisting = "Remove if the file already exists...";
std::string TARopenArchive = "Open archive for writing...";
std::string TARfinalize = "Finalize archive...";
std::string TARclose = "Close archive...";
