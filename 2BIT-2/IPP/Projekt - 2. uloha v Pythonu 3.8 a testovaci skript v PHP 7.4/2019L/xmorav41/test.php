<?php

///
/// IPP Projekt - 3. skript (test.php)
/// xmorav41
///
///

//Funkcia bubbleSort na zoradenie polí .src .in .out .rc
function bubbleSort($arr) {
    do {
        $swapped = false;
        for( $i = 0, $counter = count($arr) - 1; $i < $counter; $i++ ){
            if( $arr[$i] > $arr[$i + 1] ){
                list( $arr[$i + 1], $arr[$i] ) = array($arr[$i], $arr[$i + 1]);
                $swapped = true;
            }
        }
    }
    while($swapped);
    return $arr;
}


$shortopts = "";
$longopts  = array(
    "help",
    "directory:",
    "recursive",
    "parse-script:",
    "int-script:",
    "parse-only",
    "int-only",
    "jexamxml:"
);

$argList  = array("help","directory","recursive","parse-script","int-script","parse-only","int-only","jexamxml");

//Implicitné hodnoty
$dir = "./";
$rec = false;
$p_script = "./parse.php";
$p_only = false;
$i_script = "./interpret.py";
$i_only = false;
$j_xml = "/pub/courses/ipp/jexamxml/jexamxml.jar";

//Parsovanie argumentov
$options = getopt($shortopts, $longopts);

if (isset($options['help'])){
   if ($argc != 2){
       fwrite(STDERR, "ERROR-> Cannot call '--help' with additional arguments");
       exit(10);
   }
   else{
       fwrite(STDOUT, "\nSkript (test.php v jazyce PHP 7.4) bude sloužit pro automatické testování postupné aplikace
parse.php a interpret.py.\n");
       fwrite(STDOUT, "Parametry:\n");
       fwrite(STDOUT, "--help\n");
       fwrite(STDOUT, "--directory=path\n");
       fwrite(STDOUT, "--recursive\n");
       fwrite(STDOUT, "--parse-script=file\n");
       fwrite(STDOUT, "--int-script=file\n");
       fwrite(STDOUT, "--parse-only\n");
       fwrite(STDOUT, "--int-only\n");
       fwrite(STDOUT, "--jexamxml=file\n\n");
       exit(0);
   }
}
elseif (isset($options['parse-only'])){
    if (isset($options['int-only']) || isset($options['int-script'])){
        fwrite(STDERR, "ERROR-> Cannot call '--int-*' with '--parse-only'");
        exit(10);
    }
}
elseif (isset($options['int-only'])){
    if (isset($options['parse-only']) || isset($options['parse-script'])){
        fwrite(STDERR, "ERROR-> Cannot call '--parse-*' with '--int-only'");
        exit(10);
    }
}

if (count($argv)-1 != count($options)) {
    fwrite(STDERR, "ERROR-> Unknown argument");
    exit(10);
}

if (isset($options['directory']))
    $dir = $options['directory'];
if (isset($options['parse-script']))
    $p_script = $options['parse-script'];
if (isset($options['int-script']))
    $i_script = $options['int-script'];
if (isset($options['recursive']))
    $rec = true;
if (isset($options['parse-only']))
    $p_only = true;
if (isset($options['int-only']))
    $i_only = true;
if (isset($options['jexamxml']))
    $j_xml = $options['jexamxml'];

//Naplnenie listov súbormi z adresára, buď rekurzívne alebo nie
if ($rec) {
    exec("find " . $dir . " -regex '.*\.src$'", $srcFiles);
    exec("find " . $dir . " -regex '.*\.rc$'", $rcFiles);
    exec("find " . $dir . " -regex '.*\.in$'", $inFiles);
    exec("find " . $dir . " -regex '.*\.out'", $outFiles);
}
else {
    exec("find " . $dir . " -maxdepth 1 -regex '.*\.src$'", $srcFiles);
    exec("find " . $dir . " -maxdepth 1 -regex '.*\.rc$'", $rcFiles);
    exec("find " . $dir . " -maxdepth 1 -regex '.*\.in$'", $inFiles);
    exec("find " . $dir . " -maxdepth 1 -regex '.*\.out$'", $outFiles);
}

//Kontrola existencie .rc .in .out súborov, prípadné vytvorenie
foreach ($srcFiles as $file){
    $pathParts = pathinfo($file);
    $dirname = $pathParts['dirname'];
    $filename = $pathParts['filename'];

    $needle = $dirname."/".$filename.'.rc';
    if (!(in_array($needle,$rcFiles))){
        if ((file_put_contents($needle, "0")) === false) exit(12);
        array_push($rcFiles, $needle);
    }
    $needle = $dirname."/".$filename.'.in';
    if (!in_array($needle,$inFiles )){
        if ((file_put_contents($needle, "")) === false) exit(12);
        array_push($inFiles, $needle);
    }
    $needle = $dirname."/".$filename.'.out';
    if (!in_array($needle,$outFiles )){
        if ((file_put_contents($needle, "")) === false) exit(12);
        array_push($outFiles, $needle);
    }
}

//Zoradenie položiek listov
$srcFiles = bubbleSort($srcFiles);
$rcFiles = bubbleSort($rcFiles);
$inFiles = bubbleSort($inFiles);
$outFiles = bubbleSort($outFiles);

$counter = 0;
$success = 0;
$fail = 0;

//Inicializácia listov s hodnotami pre html
///
$htmlOrder = [];
$htmlFile = [];
$htmlPEx = [];
$htmlPGi = [];
$htmlIEx = [];
$htmlIGi = [];
$htmlPRe = [];
$htmlIRe = [];
///

//Začiatok outputu
$htmlString = "<!DOCTYPE html>
<html>
<head>
<body>
<table border=\"2\" cellpadding=\"5\" cellspacing=\"4\">";


//Parse-only
if ($p_only) {
    $htmlString .= "<tr><th>#</th><th>Test File</th><th>Parser Expected</th><th>Parser Given</th><th>Parser Result</th></tr>";
    foreach ($srcFiles as $file) {
        if (file_get_contents($rcFiles[$counter]) === false) exit(11);
        if (file_get_contents($inFiles[$counter]) === false) exit(11);
        if (file_get_contents($outFiles[$counter]) === false) exit(11);
        if (file_get_contents($file) === false) exit(11);
        $fileRC = file_get_contents($rcFiles[$counter]);
        $fileIN = file_get_contents($inFiles[$counter]);
        $fileOUT = file_get_contents($outFiles[$counter]);
        exec("php7.4 " . $p_script . " <" . $file ." > ".'xmltmp.src', $xml, $rc);


        array_push($htmlOrder, $counter + 1);
        array_push($htmlFile, $file);
        array_push($htmlPEx, $fileRC);
        array_push($htmlPGi, strval($rc));

        if ($rc == $fileRC){

            exec('java -jar '.$j_xml.' '.$outFiles[$counter].' xmltmp.src '.' delta.xml '.' ./pub/courses/ipp/jexamxml/options', $jexamxmlOut, $jexamxmlRC);
            //exec('java -jar '.$j_xml.' '.$outFiles[$counter].' xmltmp.src '.' delta.xml '.' ./jexamxml/options', $jexamxmlOut, $jexamxmlRC);


            if ($jexamxmlRC == 0) {
                ++$success;
                array_push($htmlPRe, "OK");
                $htmlString .= "<tr bgcolor=\"#5ff926\">";
            } else {
                ++$fail;
                array_push($htmlPRe, "FAIL");
                $htmlString .= "<tr bgcolor=\"#f41919\">";
            }
        }
        else{
            ++$fail;
            array_push($htmlPRe, "FAIL");
            $htmlString .= "<tr bgcolor=\"#f41919\">";
        }
        $htmlString .= "<td>$htmlOrder[$counter]</td>"
            ."<td>$htmlFile[$counter]</td>"
            ."<td>$htmlPEx[$counter]</td>"
            ."<td>$htmlPGi[$counter]</td>"
            ."<td>$htmlPRe[$counter]</td></tr>";
        if (unlink('xmltmp.src') === false) exit(12);
        $counter++;

    }
    if (unlink('delta.xml') === false) exit(12);
    $htmlString .= "<tr><td colspan='4'> Successful:</td><td style=\"text-align:RIGHT;\">$success/$counter</td></tr>";
}

//Interpret-only
elseif ($i_only){
    $htmlString .= "<tr><th>#</th><th>Test File</th><th>Interpret Expected</th><th>Interpret Given</th><th>Interpret Result</th></tr>";
    foreach ($srcFiles as $fileSRC) {
        if (file_get_contents($rcFiles[$counter]) === false) exit(11);
        if (file_get_contents($inFiles[$counter]) === false) exit(11);
        if (file_get_contents($outFiles[$counter]) === false) exit(11);
        if (file_get_contents($fileSRC) === false) exit(11);
        $fileRC = file_get_contents($rcFiles[$counter]);
        $fileIN = file_get_contents($inFiles[$counter]);
        $fileOUT = file_get_contents($outFiles[$counter]);
        $fileRC = preg_replace('/\s+/', '', $fileRC);

        exec("python3.8 " . $i_script . " --source=" . $fileSRC." --input=".$inFiles[$counter]. ' > '.'tmp.out', $out, $rcI);


        exec("diff ".'"tmp.out"'.' '.$outFiles[$counter],$out, $rcDiff);
        if (unlink("tmp.out") === false) exit(12);


        array_push($htmlOrder, $counter + 1);
        array_push($htmlFile, $fileSRC);
        array_push($htmlIEx, $fileRC);
        array_push($htmlIGi, strval($rcI));

        if (empty($rcDiff)) {
            ++$success;
            array_push($htmlIRe, "OK");
            $htmlString .= "<tr bgcolor=\"#5ff926\">";
        } else {
            ++$fail;
            array_push($htmlIRe, "FAIL");
            $htmlString .= "<tr bgcolor=\"#f41919\">";
        }
        $htmlString .= "<td>$htmlOrder[$counter]</td>"
                    ."<td>$htmlFile[$counter]</td>"
                    ."<td>$htmlIEx[$counter]</td>"
                    ."<td>$htmlIGi[$counter]</td>"
                    ."<td>$htmlIRe[$counter]</td></tr>";

        $counter++;

    }
    $htmlString .= "<tr><td colspan='4'> Successful:</td><td style=\"text-align:RIGHT;\">$success/$counter</td></tr>";
}

//Both
else{
    $htmlString .= "<tr><th>#</th><th>Test File</th><th>Parser Expected</th><th>Parser Given</th><th>Interpret Expected</th><th>Interpret Given</th><th>Parser Result</th><th>Interpret Result</th></tr>";

    foreach ($srcFiles as $file) {
        if (file_get_contents($rcFiles[$counter]) === false) exit(11);
        if (file_get_contents($inFiles[$counter]) === false) exit(11);
        if (file_get_contents($outFiles[$counter]) === false) exit(11);
        if (file_get_contents($file) === false) exit(11);
        $fileRC = file_get_contents($rcFiles[$counter]);
        $fileIN = file_get_contents($inFiles[$counter]);
        $fileOUT = file_get_contents($outFiles[$counter]);
        $fileRC = preg_replace('/\s+/', '', $fileRC);
        exec("php7.4 " . $p_script . " <" . $file ." > ".'xmltmp.src', $xml, $rcP);


        exec("python3.8 " . $i_script . " --source=xmltmp.src" ." --input=".$inFiles[$counter]. ' > '.'tmp.out', $out, $rcI);
        if (unlink('xmltmp.src') === false) exit(12);


        exec("diff tmp.out".' '.$outFiles[$counter], $diffOut, $rcDiff);
        if (unlink('tmp.out') === false) exit(12);


        array_push($htmlOrder, $counter + 1);
        array_push($htmlFile, $file);
        array_push($htmlPEx, "0");
        array_push($htmlPGi, strval($rcP));
        array_push($htmlIEx, $fileRC);
        array_push($htmlIGi, $rcI);

        if ($rcP == 0){
            array_push($htmlPRe, "OK");
            if (empty($rcDiff)){
                ++$success;
                array_push($htmlIRe, "OK");
                $htmlString .= "<tr bgcolor=\"#5ff926\">";
            }
            else{
                ++$fail;
                array_push($htmlIRe, "FAIL");
                $htmlString .= "<tr bgcolor=\"#f41919\">";
            }
        }
        else{
            ++$fail;
            array_push($htmlPRe, "FAIL");
            array_push($htmlIRe, "FAIL");
            $htmlString .= "<tr bgcolor=\"#f41919\">";
        }


        $htmlString .= "<td>$htmlOrder[$counter]</td>"
                            ."<td>$htmlFile[$counter]</td>"
                            ."<td>$htmlPEx[$counter]</td>"
                            ."<td>$htmlPGi[$counter]</td>"
                            ."<td>$htmlIEx[$counter]</td>"
                            ."<td>$htmlIGi[$counter]</td>"
                            ."<td>$htmlPRe[$counter]</td>"
                            ."<td>$htmlIRe[$counter]</td></tr>";
        $counter++;
    }
    $htmlString .= "<tr><td colspan='7'> Successful:</td><td style=\"text-align:RIGHT;\">$success/$counter</td></tr>";
}


//Koniec outputu
$htmlString .= "</table>
</body>
</html>";

echo "$htmlString";

?>