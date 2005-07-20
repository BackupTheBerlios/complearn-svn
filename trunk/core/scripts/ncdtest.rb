require 'test/unit'
require 'test/unit/ui/console/testrunner'


class CL_NCDTest < Test::Unit::TestCase
  def setup
    dmname = "testdm"
    scriptdir = File.dirname(__FILE__)
    @term1 = "horse"
    @term2 = "rider"
    @dmfiletxt="#{dmname}.txt"
    @dmfileclb="#{dmname}.clb"
    @testdir="#{scriptdir}/../examples/10-mammals"
    system("cd #{scriptdir}/..")
    err = system("make > /dev/null")
    assert(err, "CompLearn does not compile")
  end

  def teardown
    File.delete(@dmfiletxt) if File.exists?(@dmfiletxt)
    File.delete(@dmfileclb) if File.exists?(@dmfileclb)
  end

  def isNCDPairResult?(result)
    ncdmax = 1.0
    ncdmin = 0.0
    ncd = $1 if result =~ /(\d.\d+)/
    assert(ncd, "ncd value invalid")
    assert(ncd.to_f < ncdmax, "ncd value should be less than #{ncdmax}")
    assert(ncd.to_f >= ncdmin, "ncd value must be >= #{ncdmin}")
  end

  def test_clbdm
    cmd="./src/ncd #{__FILE__} #{__FILE__} -b -o #{@dmfileclb}";result=`#{cmd}`
    assert(File.exists?(@dmfileclb), "clb file not created")
    teardown
    cmd="./src/ncd -b -o #{@dmfileclb} #{__FILE__} #{__FILE__}";result=`#{cmd}`
    assert(File.exists?(@dmfileclb), "clb file not created")
  end

  def test_txtdm
    cmd="./src/ncd #{__FILE__} #{__FILE__}>#{@dmfiletxt}";result=`#{cmd}`
    assert(File.exists?(@dmfiletxt), "txt file not created")
    isNCDPairResult?(`cat #{@dmfiletxt}`)
  end

  def test_directory_mode
    cmd="./src/ncd -b -o #{@dmfileclb} -d #{@testdir} #{@testdir}"; result=`#{cmd}`
    assert(File.exists?(@dmfileclb), "clb file not created")
  end

  def test_file_mode
    # bzip
    cmd = "./src/ncd #{__FILE__} #{__FILE__}" ; result = `#{cmd}`
    cmd = "./src/ncd -f #{__FILE__} #{__FILE__}" ; result2 = `#{cmd}`
    assert(result == result2, "bzip compressor option busted")
    isNCDPairResult?(result)

    # zlib
    cmd = "./src/ncd -C zlib #{__FILE__} #{__FILE__}" ; result = `#{cmd}`
    cmd = "./src/ncd -C zlib #{__FILE__} -f #{__FILE__}" ; result2 = `#{cmd}`
    assert(result == result2, "zlib compressor option busted")
    isNCDPairResult?(result)
  end

  def test_literal_mode
    # bzip
    cmd = "./src/ncd -l horse rider" ; result = `#{cmd}`
    cmd = "./src/ncd -C bzip -l horse rider" ; result2 = `#{cmd}`
    assert(result == result2, "bzip compressor option busted")
    isNCDPairResult?(result)

    # zlib
    cmd = "./src/ncd -C zlib -l horse rider" ; result = `#{cmd}`
    cmd = "./src/ncd --compressor=zlib -l horse rider" ; result2 = `#{cmd}`
    assert(result == result2, "zlib compressor option busted")
    isNCDPairResult?(result)
  end

  def test_help
    cmd = "./src/ncd --help" ; result = `#{cmd}`
    assert(result=~/NCD OPTIONS/, "NCD help busted")
    cmd = "./src/ncd -h" ; result = `#{cmd}`
    assert(result=~/ENUMERATION/, "NCD help busted")
  end

  def test_version
    cmd = "./src/ncd --version"
    result = `#{cmd}` ; version = $1 if result=~/version (\d+.\d+.\d+)/
    assert(version, "Didn't get a version number when we were supposed to")
    cmd = "./src/ncd -V" ; result = `#{cmd}`
    assert(result=~/#{version}/, "Version numbers don't match")
  end
end

Test::Unit::UI::Console::TestRunner.run(CL_NCDTest)
