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

function generate_random_integer_array($keys, $max) {
    $result = array();
    while (--$keys >= 0) {
        $result[] = mt_rand(0, $max);
    }
    return $result;
}

function generate_random_string_array($keys, $length) {
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
    }
    return $s;
}

function test_unserialize($s) {
    print("test_unserialize: ");
    for ($i = 0; $i < 1000; ++$i) {
        $a = unserialize($s);
    }
    return $a;
}

function test_json_encode($a) {
    print("test_json_encode: ");
    for ($i = 0; $i < 1000; ++$i) {
        $j = json_encode($a);
    }
    return $j;
}

function test_json_decode($j) {
    print("test_json_decode: ");
    for ($i = 0; $i < 1000; ++$i) {
        $a = json_decode($j, True);
    }
    return $a;
}

function bench_f($f, $a) {
    $__f = create_function('$a', 'return '.$f.'($a);');
    $start = gettimeofday(True);
    $r = $__f($a);
    $past = gettimeofday(True) - $start;
    printf("%f ms\n", $past);
    return $r;
}

function bench_integer_array($keys, $size) {
    print("\ninteger, value size: $size\n");
    $a = generate_random_integer_array($keys, $size);
    $s = bench_f('test_serialize', $a);
    $r = bench_f('test_unserialize', $s);
    $j = bench_f('test_json_encode', $a);
    $r = bench_f('test_json_decode', $j);
}

function bench_string_array($keys, $length) {
    print("\nstring, value length: $length\n");
    $a = generate_random_string_array($keys, $length);
    $s = bench_f('test_serialize', $a);
    $r = bench_f('test_unserialize', $s);
    $j = bench_f('test_json_encode', $a);
    $r = bench_f('test_json_decode', $j);
}

try {

    bench_integer_array(10, 100);
    bench_integer_array(100, 10000);
    bench_integer_array(1000, 1000000);

    bench_string_array(10, 10);
    bench_string_array(100, 100);
    bench_string_array(1000, 100);
    bench_string_array(100, 1000);

} catch(Exception $e) {

}

?>
