from subprocess import Popen, PIPE

inputs = ["first word", "second word", "ABC", "56asdf6134rafj0s0f-03qk4fdf/sdf]s#sdf#mn", 'A'*255, 'B'*256, 'unknoqn_label']
outputs = ["first word explanation", "second word explanation", 'ABC explanation', '56asdf6134rafj0s0f-03qk4fdf/sdf]s#sdf#mn explanation', '255 A explanation', '', '']
errors = ["", "", "", "", '', 'Buffer was overflowed (max - 255)', 'Label doesnt exists']
exit_codes = [0, 0, 0, 0, 0, 1, 1]

bin_path = "./main"
test_output = ""

print()
print("TESTING")
print(f"script: {bin_path} ")
print("summary: ", end="")
for i in range(len(inputs)):
    process = Popen([bin_path], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    data = process.communicate(inputs[i].encode())
    if data[0].decode().strip() == outputs[i] and data[1].decode().strip() == errors[i] and process.returncode == exit_codes[i]:
        print('.', end="")
        test_output += "----------------------\n"
        test_output += f"TEST {i} PASSED\n"
        test_output += f"STDOUT: {inputs[i]}\n"
        test_output += f"STDERR: {errors[i]}\n"
        test_output += f"EXIT CODE: {exit_codes[i]}\n"
    else:
        print('F', end="")
        test_output += "----------------------\n"
        test_output += f"TEST {i} FAILED\n"
        test_output += f"OUTPUT STDOUT: {data[0].decode().strip()}\n"
        test_output += f"OUTPUT STDERR: {data[1].decode().strip()}\n"
        test_output += f"OUTPUT EXIT CODE: {process.returncode}\n"
        test_output += f"- EXPECTED STDOUT: {outputs[i]}\n"
        test_output += f"- EXPECTED STDERR: {errors[i]}\n"
        test_output += f"- EXPECTED EXIT CODE: {exit_codes[i]}\n"
test_output += "----------------------\n"
print()
print(test_output)        

