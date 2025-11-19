<?php
/*
 *IPP Projekt 2019/2020
 *@author Tomáš Moravčík xmorav41@stud.fit.vutbr.cz
 */

define("VARR", '/(L|G|T)F@(_|\-|\$|&|%|\*|!|\?|[a-zA-Z])[\S]*/');
define("SYM", '/((int|bool|string)@(?!(nil)$)(-?[1-9][0-9]*|[\S]*)|(nil@nil))/');
define("SYMVAR",'/((L|G|T)F@(_|\-|\$|&|%|\*|!|\?|[a-zA-Z])[\S]*)|(((int|bool|string)@(?!(nil)$)(-?[1-9][0-9]*|[\S]*)|(nil@nil)))/');
define("LABEL", '/^(?!(int|bool|string)$)(_|\-|\$|&|%|\*|!|\?|[a-zA-Z])[\S]*$/');
define("TYPE", '/(int|string|bool)$/');
define("COMMENT", '/#.*(\Z|\n)/');//after ins comment

//Class for storing arguments
class Arguments{
  public $stats, $stats_file;
  public $comments;
  public $jumps;
  public $labels;
  public $loc;

  public function set_value_sta($stats, $stats_file){
    $this->stats = $stats;
    $this->stats_file = $stats_file;
  }
  /*///TESTFUNC
  function get_name() {
  return $this->stats_file;
  }
  ///*/
  public function set_value_com($comments){
    $this->comments = $comments;
  }
  public function set_value_jum($jumps){
    $this->jumps = $jumps;
  }
  public function set_value_lab($labels){
    $this->labels = $labels;
  }
  public function set_value_loc($loc){
    $this->loc = $loc;
  }

}

//@brief Process given arguments
//@params argc - number of arguments
//@params argv - array of arguments
//@params argc - class for storage
//@return Outputs filled Argmuments class
function Process_args($argc,$argv,$args){
  if ($argc == 1){
    return;
  }
  elseif(($argc == 2) && ($argv[1] == "--help")){
    echo "\n";
    echo "Skript typu filtr (parse.php v jazyce PHP 7.4) načte ze standardního vstupu zdrojový kód v IPP-code20 (viz sekce 6), zkontroluje lexikální a syntaktickou správnost kódu a vypíše na standardní výstup XML reprezentaci programu.\n";
    echo "\n";
    exit(0);
  }
  elseif (($argc == 2) && (preg_match('/--stats=.*/',$argv[1]))) {
    $stats = true;
    $file_name = preg_replace('/--stats=/','',$argv[1]);
    $args->set_value_sta($stats,$file_name);
    //echo "Štatistika: \n";
    return $args;
  }
  elseif (($argc > 2) && (preg_grep('/--stats=.*/',$argv))){
    $stats = $comments = $jumps = $labels = $loc = false;
    for($i = 1;$i < $argc; $i++) {

      if ((preg_match('/--stats=.*/',$argv[$i])) && $stats == false){
        $stats = true;
        $file_name = preg_replace('/--stats=/','',$argv[$i]);
        $args->set_value_sta($stats,$file_name);
        //echo "Štatistika: \n";
      }
      elseif ($argv[$i] == "--comments" && $comments == false){
        $comments = true;
        $args->set_value_com($comments);
        //echo "Coms: \n";
      }
      elseif ($argv[$i] == "--jumps" && $jumps == false){
        $jumps = true;
        $args->set_value_jum($jumps);
        //echo "Jumps: \n";
      }
      elseif ($argv[$i] == "--labels" && $labels == false){
        $labels = true;
        $args->set_value_lab($labels);
        //echo "Labs: \n";
      }
      elseif ($argv[$i] == "--loc" && $loc == false){
        $loc = true;
        $args->set_value_loc($loc);
        //echo "Loc: \n";
      }
      else {
        echo "Incorrect $i. argument '$argv[$i]'\n";
        exit(10);
      }
    }
    return $args;
  }
  else {
    echo "Unknown argumets\n";
    exit(10);
  }
}

//@brief Error handling
//@params line - line including error
//@params code - error code
//@return Exits with error code
function Exit_error_lex($line,$code){
  echo "Error $code on line '$line' \n";
  exit($code);
}
//@brief Type check [later updated for different types]
//@params string - input string to check type
//@return Result string with type
function Sym_or_var($string){
  if (preg_match(SYM,$string) == 1){
    $string = preg_replace("/@(nil|-?[1-9][0-9]*|[\S]*)/","",$string);
    return $string;
  }
  elseif (preg_match(VARR,$string) == 1){
    return "var";
  }
  elseif (preg_match(LABEL,$string) == 1){
    return "label";
  }
  elseif (preg_match(TYPE,$string) == 1){
    return $string;
  }
}
//@brief Generates xml code for instruction
//@params xml - handle for xml code
//@params program - handle for xml code
//@params counter - instruction order
//@params arg0 - instruction name
//@params arg1 - first instruction argument
//@params arg2 - second instruction argument
//@params arg2 - third instruction argument
function Generate_inst($xml,$program,$counter,$arg0,$arg1,$arg2,$arg3){
  $instruction = $xml->createElement("instruction");
  $instruction->setAttribute("order","$counter");
  $instruction->setAttribute("opcode","$arg0");

  if ($arg1 != NULL){
    $string1 = Sym_or_var($arg1);
    if ($string1 == "var"){//var type inst
      $arg11 = $xml->createElement("arg1",htmlspecialchars($arg1));
      $arg11->setAttribute("type","var");
      $instruction->appendChild($arg11);
    }
    elseif(preg_match(TYPE,$string1) == 1){//int/bool/string type inst
      $arg1_piece = preg_split("/[@]/", $arg1, 2);
      $arg11 = $xml->createElement("arg1",$arg1_piece[1]);
      $arg11->setAttribute("type",htmlspecialchars($string1));
      $instruction->appendChild($arg11);
    }
    elseif(preg_match('/(label)/',$string1) == 1){ //label type inst
      $arg11 = $xml->createElement("arg1",htmlspecialchars($arg1));
      $arg11->setAttribute("type",$string1);
      $instruction->appendChild($arg11);
    }
    else {//sym, seemingly obsolete here [actually nil]
      $arg1_piece = preg_split("/[@]/", $arg1, 2);
      $arg11 = $xml->createElement("arg1",htmlspecialchars($arg1_piece[1]));
      $arg11->setAttribute("type",$string1);
      $instruction->appendChild($arg11);
    }

  }
  if ($arg2 != NULL){
    $string2 = Sym_or_var($arg2);
    if ($string2 == "var"){//var type inst
      $arg22 = $xml->createElement("arg2",htmlspecialchars($arg2));
      $arg22->setAttribute("type","var");
      $instruction->appendChild($arg22);
    }
    elseif(preg_match(SYM,$arg2) == 1){//int/bool/string type inst
      $arg2_piece = preg_split("/[@]/", $arg2, 2);
      $arg22 = $xml->createElement("arg2",$arg2_piece[1]);
      $arg22->setAttribute("type",$string2);
      $instruction->appendChild($arg22);
    }
    elseif(preg_match('/(label)/',$string2) == 1){ //label type inst
      $arg22 = $xml->createElement("arg2",htmlspecialchars($arg2));
      $arg22->setAttribute("type","label");
      $instruction->appendChild($arg22);
    }
    elseif(preg_match(TYPE,$arg2) == 1){//int/bool/string type inst
      $arg22 = $xml->createElement("arg2");
      $arg22->setAttribute("type",$arg2);
      $instruction->appendChild($arg22);
    }
  }
  if ($arg3 != NULL){
    $string3 = Sym_or_var($arg3);
    if ($string3 == "var"){//var type inst
      $arg33 = $xml->createElement("arg3",htmlspecialchars($arg3));
      $arg33->setAttribute("type","var");
      $instruction->appendChild($arg33);
    }
    elseif(preg_match(TYPE,$string2) == 1){//int/bool/string type inst
      $arg3_piece = preg_split("/[@]/", $arg3, 2);
      $arg33 = $xml->createElement("arg3",$string3);
      $arg33->setAttribute("type","type");
      $instruction->appendChild($arg33);
    }
    elseif(preg_match('/(label)/',$string3) == 1){ //label type inst
      $arg33 = $xml->createElement("arg3",htmlspecialchars($arg3));
      $arg33->setAttribute("type",$string3);
      $instruction->appendChild($arg33);
    }
    else {//sym, seemingly obsolete here [actually nil]
      $arg3_piece = preg_split("/[@]/", $arg3, 2);
      $arg33 = $xml->createElement("arg3",htmlspecialchars($arg3_piece[1]));
      $arg33->setAttribute("type",$string3);
      $instruction->appendChild($arg33);
    }

  }

  $program->appendChild($instruction);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

$args = new Arguments();
$args = Process_args($argc,$argv,$args);
/////////////////////////////////
$f = fopen('php://stdin', 'r');
$line = fgets($f);
$line = preg_replace(COMMENT,'',$line);//deletes comments
//many comments at the beginning
if ($line == "\n"){
  while (empty($line == 1)){
    $line = fgets($f);
    $line = preg_replace(COMMENT,'',$line);
    if (!empty($line) && $line != "\n"){
      break;
    }
  }
}
//header check//
if (trim($line) != ".IPPcode20"){
    echo "Missing header, '$line' instead of '.IPPcode20'";
    exit(21);
}

/////////////////////////////////
$xml = new DOMDocument('1.0','UTF-8');
$xml->formatOutput = true;
$program = $xml->createElement("program");
$program->setAttribute("languague","IPPcode20");
$xml->appendChild($program);
/////////////////////////////////
$count = 1;
while($line = fgets($f)) {
  $line = preg_replace(COMMENT,'',$line);//deletes comments
  $line_piece = preg_split("/[\s]+/", $line);//splits instruction into name and args for further processing
  $line_piece[0] = strtoupper($line_piece[0]);


  switch($line_piece[0]){
    /////////////////////
    // <>
    case "BREAK":
    case "CREATEFRAME":
    case "POPFRAME":
    case "PUSHFRAME":
    case "RETURN":
    if (!empty($line_piece[1]))
    {
      Exit_error_lex($line,23);
    }
    else
    {
      Generate_inst($xml,$program,$count,$line_piece[0],NULL,NULL,NULL);
    }
      break;
    /////////////////////
    //<VAR>
    case "DEFVAR":
    case "POPS":
    if (preg_match(VARR,$line_piece[1]) == 0 || (!empty($line_piece[2])) )
    {
      Exit_error_lex($line,23);
    }
    else
    {
      Generate_inst($xml,$program,$count,$line_piece[0],$line_piece[1],NULL,NULL);
    }
      break;
    /////////////////////
    //<SYMB>
    case "DPRINT":
    case "EXIT":
    case "PUSHS":
    case "WRITE":
      if ( preg_match((SYMVAR),$line_piece[1]) == 0  || (!empty($line_piece[2])) )
      {
        Exit_error_lex($line,23);
      }
      else
      {
        Generate_inst($xml,$program,$count,$line_piece[0],$line_piece[1],NULL,NULL);
      }
      break;
    /////////////////////
    //<LABEL>
    case "CALL":
    case "LABEL":
    case "JUMP":
      if (preg_match(LABEL,$line_piece[1])  == 0 || (!empty($line_piece[2])) )
      {
        Exit_error_lex($line,23);
      }
      else
      {
        Generate_inst($xml,$program,$count,$line_piece[0],$line_piece[1],NULL,NULL);
      }
      break;
    /////////////////////
    //<VAR> <SYMB>
    case "INT2CHAR":
    case "MOVE":
    case "NOT":
    case "STRLEN":
    case "TYPE":
      if (preg_match(VARR,$line_piece[1])  == 0 || preg_match((SYMVAR),$line_piece[2]) == 0 || (!empty($line_piece[3])))
      {
        Exit_error_lex($line,23);
      }
      else
      {
        Generate_inst($xml,$program,$count,$line_piece[0],$line_piece[1],$line_piece[2],NULL);
      }
      break;
    /////////////////////
    //<VAR><TYPE>
    case "READ":
      if (preg_match(VARR,$line_piece[1])  == 0 || preg_match(TYPE,$line_piece[2]) == 0 || (!empty($line_piece[3])) )
      {
        Exit_error_lex($line,23);
      }
      else
      {
        Generate_inst($xml,$program,$count,$line_piece[0],$line_piece[1],$line_piece[2],NULL);
      }
      break;
    /////////////////////
    //<VAR><SYMB1><SYMB2>
    case "ADD":
    case "SUB":
    case "MUL":
    case "IDIV":
    case "LT":
    case "GT":
    case "EQ":
    case "AND":
    case "OR":
    case "CONCAT":
    case "GETCHAR":
    case "SETCHAR":
    case "STRI2INT":
      if (preg_match(VARR,$line_piece[1]) == 0 || preg_match((SYMVAR),$line_piece[2]) == 0 || preg_match((SYMVAR),$line_piece[3]) == 0 || (!empty($line_piece[4])))
      {
        Exit_error_lex($line,23);
      }
      else
      {
        Generate_inst($xml,$program,$count,$line_piece[0],$line_piece[1],$line_piece[2],$line_piece[3]);
      }
      break;
    /////////////////////
    //<LABEL><SYMB1><SYMB2>
    case "JUMPIFEQ":
    case "JUMPIFNEQ":
      if (preg_match(LABEL,$line_piece[1])  == 0 || preg_match((SYMVAR),$line_piece[2]) == 0 || preg_match((SYMVAR),$line_piece[3]) == 0 || (!empty($line_piece[4])))
      {
        Exit_error_lex($line,23);
      }
      else
      {
        Generate_inst($xml,$program,$count,$line_piece[0],$line_piece[1],$line_piece[2],$line_piece[3]);
      }
        break;
    //void after comment deletion
    case "":
      $count--;
      break;
    default:
      echo "Unknown instruction ";
      Exit_error_lex($line,23);
  }
  $count++;
}
/////////////////////////////////
fclose( $f );

echo $xml->saveXML();

?>
