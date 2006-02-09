require File.dirname(__FILE__) + '/../test_helper'

class LabTest < Test::Unit::TestCase
  fixtures :labs

  def setup
    @lab = Lab.find(1)
  end

  # Replace this with your real tests.
  def test_truth
    assert_kind_of Lab,  @lab
  end
end
