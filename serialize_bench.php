<?php

function generate_random_string($length) {
    $result = '';
    $chars = '0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_';
    $rand_max = strlen($chars) - 1;
    mt_srand();
    while (--$length >= 0) {
        $result .= $chars[mt_rand(0, $rand_max)];
    }
    return $result;
}

function generate_array($keys, $length) {
    $result = array();
    while (--$keys >= 0) {
        $key = generate_random_string(10);
        $result[$key] = generate_random_string($length);
    }
    return $result;
}

function test_serialize($a) {
    print("test_serialize: ");
    for ($i = 0; $i < 1000; ++$i) {
        $s = serialize($a);
        $u = unserialize($s);
    }
}

function test_json($a) {
    print("test_json: ");
    for ($i = 0; $i < 1000; ++$i) {
        $s = json_encode($a);
        $u = json_decode($s, True);
    }
}

function test_var_export($a) {
    print("test_var_export: ");
    for ($i = 0; $i < 1000; ++$i) {
        $s = var_export($a, True);
        eval('$u='.$s.';');
    }
}

function bench($f, $a) {
    $__f = create_function('$a', $f.'($a);');
    $start = gettimeofday(True);
    $__f($a);
    $past = gettimeofday(True) - $start;
    printf("%f ms\n", $past);
}

try {

    print("\nvalue length: 100\n");
    $a = generate_array(100, 100);
    bench('test_serialize', $a);
    bench('test_json', $a);
    bench('test_var_export', $a);

    print("\nvalue length: 1000\n");
    $a = generate_array(100, 1000);
    bench('test_serialize', $a);
    bench('test_json', $a);
    bench('test_var_export', $a);

} catch(Exception $e) {

}

?>
