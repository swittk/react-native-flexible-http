require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

Pod::Spec.new do |s|
  s.name         = "react-native-flexible-http"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  s.platforms    = { :ios => "10.0" }
  s.source       = { :git => "https://github.com/swittk/react-native-flexible-http.git", :tag => "#{s.version}" }

  s.source_files = "ios/**/*.{h,m,mm}", "cpp/**/*.{h,cpp}"

  s.dependency "React-Core"
  # Need to include this to include turbomodules... stupid me couldn't figure how to include it
  # inspired by https://github.com/software-mansion/react-native-reanimated/blob/cec101716d5690f51187c0f739b3ea3e7080440e/RNReanimated.podspec#L113
  s.dependency 'ReactCommon/turbomodule/core'
#  s.dependency "Folly"
end
