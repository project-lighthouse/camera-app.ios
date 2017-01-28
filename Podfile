# Uncomment the next line to define a global platform for your project
platform :ios, '10.0'

workspace 'Lighthouse Camera'

target 'Lighthouse Camera' do
  # Comment the next line if you're not using Swift and don't want to use dynamic frameworks
  use_frameworks!

  # Pods for Lighthouse Camera
  pod 'OpenCV', '~> 3.x'

  target 'Lighthouse CameraTests' do
    inherit! :search_paths
    # Pods for testing
  end

  target 'Lighthouse CameraUITests' do
    inherit! :search_paths
    # Pods for testing
  end

end

post_install do |installer|
  # Disable code coverage for all Pods and Pods Project
  installer.pods_project.targets.each do |target|
      target.build_configurations.each do |config|
          config.build_settings['CLANG_ENABLE_CODE_COVERAGE'] = 'NO'
      end
  end
  installer.pods_project.build_configurations.each do |config|
      config.build_settings['CLANG_ENABLE_CODE_COVERAGE'] = 'NO'
  end
end
