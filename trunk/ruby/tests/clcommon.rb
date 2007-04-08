module CompLearn
class CSCommand
  def execute()
    puts "executed!"
  end
  def to_s
    "cscommand test to_s at #{self.object_id}"
  end
end
class BCommand
end
class BVersion < BCommand
  def execute()
    CompLearn.version
  end
end
end
