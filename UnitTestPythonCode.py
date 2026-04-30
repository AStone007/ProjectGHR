import ctypes
import math

wf = ctypes.CDLL("./wf.dll")
class WaveformSample(ctypes.Structure):
    _fields_ = [
        ("timestamp", ctypes.c_double),
        ("phase_A_voltage", ctypes.c_double),
        ("phase_B_voltage", ctypes.c_double),
        ("phase_C_voltage", ctypes.c_double),
        ("line_current", ctypes.c_double),
        ("frequency", ctypes.c_double),
        ("power_factor", ctypes.c_double),
        ("thd_percent", ctypes.c_double)]

class File(ctypes.Structure):
    _fields_ = [
        ("samples", ctypes.POINTER(WaveformSample)),
        ("num_samples", ctypes.c_int)]
PHASE_A = 0
PHASE_B = 1
PHASE_C = 2


# C FUNCTIONS
wf.compute_rms.argtypes = [ctypes.POINTER(File), ctypes.c_int]
wf.compute_rms.restype = ctypes.c_double
wf.compute_peak_to_peak.argtypes = [ctypes.POINTER(File), ctypes.c_int]
wf.compute_peak_to_peak.restype = ctypes.c_double
wf.compute_dc_offset.argtypes = [ctypes.POINTER(File), ctypes.c_int]
wf.compute_dc_offset.restype = ctypes.c_double
wf.compute_std_dev.argtypes = [ctypes.POINTER(File), ctypes.c_int]
wf.compute_std_dev.restype = ctypes.c_double
wf.check_compliance.argtypes = [ctypes.c_double]
wf.check_compliance.restype = ctypes.c_int
wf.power_factor.argtypes = [ctypes.POINTER(File)]
wf.power_factor.restype = ctypes.c_double
wf.thd_percent.argtypes = [ctypes.POINTER(File)]
wf.thd_percent.restype = ctypes.c_double
wf.frequency.argtypes = [ctypes.POINTER(File)]
wf.frequency.restype = ctypes.c_double


# create file
def create_file(samples_list):
    array_type = WaveformSample * len(samples_list)
    c_array = array_type(*samples_list)
    file = File()
    file.samples = c_array
    file.num_samples = len(samples_list)
    return file


test_counter = 1

# format output
def test(name, description, actual, expected):
    global test_counter
    passed = (actual == expected)
    print(f"Unit Test Case {test_counter} ({description}):")
    print(f"Input: {name}")
    print(f"Expected Output: {expected}")
    print(f"Observed Output: {actual}")
    print(f"Result: {'PASSED' if passed else 'FAILED'}\n")
    test_counter += 1
    return passed


# NORMAL TESTS

file = create_file([
    WaveformSample(0, 10, 20, -10, 0, 50, 0.9, 2),
    WaveformSample(1, -10, 20, -10, 0, 50, 0.9, 2)])

# RMS
test("RMS Phase A", "RMS of alternating values (+10, -10) should be 10",
     wf.compute_rms(ctypes.byref(file), PHASE_A), 10)
test("RMS Phase B", "RMS of constant value (20, 20) should be 20",
     wf.compute_rms(ctypes.byref(file), PHASE_B), 20)
test("RMS Phase C", "RMS of constant negative value (-10, -10) should be 10",
     wf.compute_rms(ctypes.byref(file), PHASE_C), 10)

# Peak-to-peak
test("P2P Phase A", "Peak-to-peak of values (+10, -10) should be 20",
     wf.compute_peak_to_peak(ctypes.byref(file), PHASE_A), 20)
test("P2P Phase B", "Peak-to-peak of constant values (20, 20) should be 0",
     wf.compute_peak_to_peak(ctypes.byref(file), PHASE_B), 0)
test("P2P Phase C", "Peak-to-peak of constant values (-10, -10) should be 0",
     wf.compute_peak_to_peak(ctypes.byref(file), PHASE_C), 0)

# DC offset
test("DC Offset Phase A", "Mean of (+10, -10) should be 0",
     wf.compute_dc_offset(ctypes.byref(file), PHASE_A), 0)
test("DC Offset Phase B", "Mean of constant values (20, 20) should be 20",
     wf.compute_dc_offset(ctypes.byref(file), PHASE_B), 20)
test("DC Offset Phase C", "Mean of constant values (-10, -10) should be -10",
     wf.compute_dc_offset(ctypes.byref(file), PHASE_C), -10)

# Std dev
test("STD Phase A", "Standard deviation of (+10, -10) should be 10",
     wf.compute_std_dev(ctypes.byref(file), PHASE_A), 10)
test("STD Phase B", "Standard deviation of constant values should be 0",
     wf.compute_std_dev(ctypes.byref(file), PHASE_B), 0)
test("STD Phase C", "Standard deviation of constant values should be 0",
     wf.compute_std_dev(ctypes.byref(file), PHASE_C), 0)

# Other functions
test("Compliance valid", "Voltage 230V should be within acceptable limits",
     wf.check_compliance(230), 1)
test("Compliance invalid", "Voltage 100V should be outside acceptable limits",
     wf.check_compliance(100), 0)
test("Power Factor", "Power factor should return 0.9 from dataset",
     wf.power_factor(ctypes.byref(file)), 0.9)
test("THD", "THD should return 2 from dataset",
     wf.thd_percent(ctypes.byref(file)), 2)
test("Frequency", "Frequency should return 50Hz",
     wf.frequency(ctypes.byref(file)), 50)


# EDGE TESTS

empty_file = create_file([])
test("RMS Empty File", "RMS should return 0 for empty file (no samples)",
     wf.compute_rms(ctypes.byref(empty_file), PHASE_A), 0)
single_file = create_file([WaveformSample(0, 5, 5, 5, 0, 50, 1, 0)])
test("RMS Single Value", "RMS of single value should be 5",
     wf.compute_rms(ctypes.byref(single_file), PHASE_A), 5)
test("P2P Single Value", "Peak-to-peak of single value should be 0",
     wf.compute_peak_to_peak(ctypes.byref(single_file), PHASE_A), 0)
zero_file = create_file([WaveformSample(0, 0, 0, 0, 0, 50, 0, 0)])
test("STD All Zeros", "Standard deviation of all zeros should be 0",
     wf.compute_std_dev(ctypes.byref(zero_file), PHASE_A), 0)
extreme_file = create_file([WaveformSample(0, 1e6, -1e6, 1e6, 0, 50, 1, 0)])
test("RMS Extreme Values", "RMS should handle very large values correctly",
     wf.compute_rms(ctypes.byref(extreme_file), PHASE_A), 1e6)


# ROBUSTNESS TESTS

def safe_call(func, *args):
    try:
        return func(*args), None
    except:
        return None, "ERROR"


#overflow buffer
MAX_CLIPS = 1000
large_samples = []
for i in range(1500):
    large_samples.append(WaveformSample(i, 1000, 1000, 1000, 0, 50, 1, 0))
    
large_file = create_file(large_samples)
clip, err = safe_call(wf.count_clipped, ctypes.pointer(large_file), 325)
overflow_detected = (
    clip is None or
    clip.count_A > MAX_CLIPS or
    clip.count_B > MAX_CLIPS or
    clip.count_C > MAX_CLIPS    )
test("Overflow Test",
     "Clipping should handle large datasets safely",
     "PASSED" if not overflow_detected else "FAILED",
     "PASSED")


#invalid phase
result, err = safe_call(wf.compute_rms, ctypes.byref(file), 999)
test("Invalid Phase RMS", "Function should handle invalid phase input",
     result if result is not None else "FAILED", 0)


#NaN input
nan_file = create_file([WaveformSample(0, float('nan'), 0, 0, 0, 50, 1, 0)])
result, err = safe_call(wf.compute_rms, ctypes.byref(nan_file), PHASE_A)
test("NaN Handling RMS", "RMS should handle NaN safely",
     result if result is not None else "FAILED", 0)


#infinite value
inf_file = create_file([WaveformSample(0, float('inf'), 0, 0, 0, 50, 1, 0)])
result, err = safe_call(wf.compute_rms, ctypes.byref(inf_file), PHASE_A)
test("Infinite Value RMS", "RMS should handle infinite values safely",
     result if result is not None else "FAILED", 0)


#negative frequency
bad_freq_file = create_file([WaveformSample(0, 0, 0, 0, 0, -50, 1, 0)])
result, err = safe_call(wf.frequency, ctypes.byref(bad_freq_file))
test("Negative Frequency",
     "Frequency should reject negative values",
     result if result is not None else "FAILED",
     50)
