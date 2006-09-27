require File.dirname(__FILE__) + '/../test_helper'
require 'clo_controller'

# Re-raise errors caught by the controller.
class CloController; def rescue_action(e) raise e end; end

class CloControllerTest < Test::Unit::TestCase
  def setup
    @controller = CloController.new
    @request    = ActionController::TestRequest.new
    @response   = ActionController::TestResponse.new
  end

  # Replace this with your real tests.
  def test_truth
    assert true
  end
end
