/* конфигурационный файл softfloat
 * для компиляции в составе с inanity2
 * написан вручную, по инструкциям из Makefile
 */

exports.configureCompiler = function(objectFile, compiler) {
	// объектные файлы: <conf>/object
	var a = /^([^\/]+)\/((cpp\.)?([^\/]+))$/.exec(objectFile);
	compiler.configuration = a[1];
	if(a[3])
		compiler.setSourceFile(a[4] + '.cpp');
	else {
		compiler.setSourceFile('softfloat/source/' + a[2].replace(/\./g, '/') + '.c');
		compiler.cppMode = false;
		compiler.strict = false;
	}
	compiler.addIncludeDir('softfloat/source/8086');
	compiler.addIncludeDir('softfloat/source/include');
};

var objects = [
'cpp.softfloat',
's_eq128',
's_le128',
's_lt128',
's_shortShiftLeft128',
's_shortShiftRight128',
's_shortShiftRightJam64',
's_shortShiftRightJam64Extra',
's_shortShiftRightJam128',
's_shortShiftRightJam128Extra',
's_shiftRightJam32',
's_shiftRightJam64',
's_shiftRightJam64Extra',
's_shiftRightJam128',
's_shiftRightJam128Extra',
's_shiftRightJam256M',
's_countLeadingZeros8',
's_countLeadingZeros16',
's_countLeadingZeros32',
's_countLeadingZeros64',
's_add128',
's_add256M',
's_sub128',
's_sub256M',
's_mul64ByShifted32To128',
's_mul64To128',
's_mul128By32',
's_mul128To256M',
's_approxRecip_1Ks',
's_approxRecip32_1',
's_approxRecipSqrt_1Ks',
's_approxRecipSqrt32_1',
'8086.softfloat_raiseFlags',
'8086.s_f16UIToCommonNaN',
'8086.s_commonNaNToF16UI',
'8086.s_propagateNaNF16UI',
'8086.s_f32UIToCommonNaN',
'8086.s_commonNaNToF32UI',
'8086.s_propagateNaNF32UI',
'8086.s_f64UIToCommonNaN',
'8086.s_commonNaNToF64UI',
'8086.s_propagateNaNF64UI',
'8086.extF80M_isSignalingNaN',
'8086.s_extF80UIToCommonNaN',
'8086.s_commonNaNToExtF80UI',
'8086.s_propagateNaNExtF80UI',
'8086.f128M_isSignalingNaN',
'8086.s_f128UIToCommonNaN',
'8086.s_commonNaNToF128UI',
'8086.s_propagateNaNF128UI',
's_roundPackToUI32',
's_roundPackToUI64',
's_roundPackToI32',
's_roundPackToI64',
's_normSubnormalF16Sig',
's_roundPackToF16',
's_normRoundPackToF16',
's_addMagsF16',
's_subMagsF16',
's_mulAddF16',
's_normSubnormalF32Sig',
's_roundPackToF32',
's_normRoundPackToF32',
's_addMagsF32',
's_subMagsF32',
's_mulAddF32',
's_normSubnormalF64Sig',
's_roundPackToF64',
's_normRoundPackToF64',
's_addMagsF64',
's_subMagsF64',
's_mulAddF64',
's_normSubnormalExtF80Sig',
's_roundPackToExtF80',
's_normRoundPackToExtF80',
's_addMagsExtF80',
's_subMagsExtF80',
's_normSubnormalF128Sig',
's_roundPackToF128',
's_normRoundPackToF128',
's_addMagsF128',
's_subMagsF128',
's_mulAddF128',
'softfloat_state',
'ui32_to_f16',
'ui32_to_f32',
'ui32_to_f64',
'ui32_to_extF80',
'ui32_to_extF80M',
'ui32_to_f128',
'ui32_to_f128M',
'ui64_to_f16',
'ui64_to_f32',
'ui64_to_f64',
'ui64_to_extF80',
'ui64_to_extF80M',
'ui64_to_f128',
'ui64_to_f128M',
'i32_to_f16',
'i32_to_f32',
'i32_to_f64',
'i32_to_extF80',
'i32_to_extF80M',
'i32_to_f128',
'i32_to_f128M',
'i64_to_f16',
'i64_to_f32',
'i64_to_f64',
'i64_to_extF80',
'i64_to_extF80M',
'i64_to_f128',
'i64_to_f128M',
'f16_to_ui32',
'f16_to_ui64',
'f16_to_i32',
'f16_to_i64',
'f16_to_ui32_r_minMag',
'f16_to_ui64_r_minMag',
'f16_to_i32_r_minMag',
'f16_to_i64_r_minMag',
'f16_to_f32',
'f16_to_f64',
'f16_to_extF80',
'f16_to_extF80M',
'f16_to_f128',
'f16_to_f128M',
'f16_roundToInt',
'f16_add',
'f16_sub',
'f16_mul',
'f16_mulAdd',
'f16_div',
'f16_rem',
'f16_sqrt',
'f16_eq',
'f16_le',
'f16_lt',
'f16_eq_signaling',
'f16_le_quiet',
'f16_lt_quiet',
'f16_isSignalingNaN',
'f32_to_ui32',
'f32_to_ui64',
'f32_to_i32',
'f32_to_i64',
'f32_to_ui32_r_minMag',
'f32_to_ui64_r_minMag',
'f32_to_i32_r_minMag',
'f32_to_i64_r_minMag',
'f32_to_f16',
'f32_to_f64',
'f32_to_extF80',
'f32_to_extF80M',
'f32_to_f128',
'f32_to_f128M',
'f32_roundToInt',
'f32_add',
'f32_sub',
'f32_mul',
'f32_mulAdd',
'f32_div',
'f32_rem',
'f32_sqrt',
'f32_eq',
'f32_le',
'f32_lt',
'f32_eq_signaling',
'f32_le_quiet',
'f32_lt_quiet',
'f32_isSignalingNaN',
'f64_to_ui32',
'f64_to_ui64',
'f64_to_i32',
'f64_to_i64',
'f64_to_ui32_r_minMag',
'f64_to_ui64_r_minMag',
'f64_to_i32_r_minMag',
'f64_to_i64_r_minMag',
'f64_to_f16',
'f64_to_f32',
'f64_to_extF80',
'f64_to_extF80M',
'f64_to_f128',
'f64_to_f128M',
'f64_roundToInt',
'f64_add',
'f64_sub',
'f64_mul',
'f64_mulAdd',
'f64_div',
'f64_rem',
'f64_sqrt',
'f64_eq',
'f64_le',
'f64_lt',
'f64_eq_signaling',
'f64_le_quiet',
'f64_lt_quiet',
'f64_isSignalingNaN',
'extF80_to_ui32',
'extF80_to_ui64',
'extF80_to_i32',
'extF80_to_i64',
'extF80_to_ui32_r_minMag',
'extF80_to_ui64_r_minMag',
'extF80_to_i32_r_minMag',
'extF80_to_i64_r_minMag',
'extF80_to_f16',
'extF80_to_f32',
'extF80_to_f64',
'extF80_to_f128',
'extF80_roundToInt',
'extF80_add',
'extF80_sub',
'extF80_mul',
'extF80_div',
'extF80_rem',
'extF80_sqrt',
'extF80_eq',
'extF80_le',
'extF80_lt',
'extF80_eq_signaling',
'extF80_le_quiet',
'extF80_lt_quiet',
'extF80_isSignalingNaN',
'extF80M_to_ui32',
'extF80M_to_ui64',
'extF80M_to_i32',
'extF80M_to_i64',
'extF80M_to_ui32_r_minMag',
'extF80M_to_ui64_r_minMag',
'extF80M_to_i32_r_minMag',
'extF80M_to_i64_r_minMag',
'extF80M_to_f16',
'extF80M_to_f32',
'extF80M_to_f64',
'extF80M_to_f128M',
'extF80M_roundToInt',
'extF80M_add',
'extF80M_sub',
'extF80M_mul',
'extF80M_div',
'extF80M_rem',
'extF80M_sqrt',
'extF80M_eq',
'extF80M_le',
'extF80M_lt',
'extF80M_eq_signaling',
'extF80M_le_quiet',
'extF80M_lt_quiet',
'f128_to_ui32',
'f128_to_ui64',
'f128_to_i32',
'f128_to_i64',
'f128_to_ui32_r_minMag',
'f128_to_ui64_r_minMag',
'f128_to_i32_r_minMag',
'f128_to_i64_r_minMag',
'f128_to_f16',
'f128_to_f32',
'f128_to_extF80',
'f128_to_f64',
'f128_roundToInt',
'f128_add',
'f128_sub',
'f128_mul',
'f128_mulAdd',
'f128_div',
'f128_rem',
'f128_sqrt',
'f128_eq',
'f128_le',
'f128_lt',
'f128_eq_signaling',
'f128_le_quiet',
'f128_lt_quiet',
'f128_isSignalingNaN',
'f128M_to_ui32',
'f128M_to_ui64',
'f128M_to_i32',
'f128M_to_i64',
'f128M_to_ui32_r_minMag',
'f128M_to_ui64_r_minMag',
'f128M_to_i32_r_minMag',
'f128M_to_i64_r_minMag',
'f128M_to_f16',
'f128M_to_f32',
'f128M_to_extF80M',
'f128M_to_f64',
'f128M_roundToInt',
'f128M_add',
'f128M_sub',
'f128M_mul',
'f128M_mulAdd',
'f128M_div',
'f128M_rem',
'f128M_sqrt',
'f128M_eq',
'f128M_le',
'f128M_lt',
'f128M_eq_signaling',
'f128M_le_quiet',
'f128M_lt_quiet',
];

exports.configureComposer = function(libraryFile, composer) {
	// файлы библиотек: <conf>/library
	var a = /^(([^\/]+)\/)([^\/]+)$/.exec(libraryFile);
	var confDir = a[1];
	composer.configuration = a[2];
	for ( var i = 0; i < objects.length; ++i)
		composer.addObjectFile(confDir + objects[i]);
};