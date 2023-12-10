import ExpoModulesCore
import Foundation
import MLKitTextRecognition
import MLKitVision

let ERROR_DOMAIN: String = "red.infinite.react-native-mlkit.ImageLabelerErrorDomain"

public enum RNMLKitImageError: Error {
  case invalidImageURL(imagePath: String)
  case couldNotLoadImage(imagePath: String)
}

public class RNMLKitImage {
  public var imageURL: URL
  public var uiImage: UIImage
  public var visionImage: VisionImage

  public init(imagePath: String) throws {
    guard let imageURL = URL(string: imagePath) else {
      throw RNMLKitImageError.invalidImageURL(imagePath: imagePath)
    }

    self.imageURL = imageURL

    guard let uiImage = UIImage(contentsOfFile: imageURL.path) else {
      throw RNMLKitImageError.couldNotLoadImage(imagePath: imageURL.path)
    }

    self.uiImage = uiImage

    visionImage = VisionImage(image: uiImage)
    visionImage.orientation = uiImage.imageOrientation

  }
}

public class MlKitWrapperModule: Module {
  // private var textRecognizer = MLKitTextRecognition.

  // TextRecognizer.textRecognizer(options: TextRecognizerOptions())

  // Each module class must implement the definition function. The definition consists of components
  // that describes the module's functionality and behavior.
  // See https://docs.expo.dev/modules/module-api for more details about available components.
  public func definition() -> ModuleDefinition {
    // Sets the name of the module that JavaScript code will use to refer to the module. Takes a string as an argument.
    // Can be inferred from module's class name, but it's recommended to set it explicitly for clarity.
    // The module will be accessible from `requireNativeModule('MlKitWrapper')` in JavaScript.
    Name("MlKitWrapper")

    // Defines a JavaScript function that always returns a Promise and whose native code
    // is by default dispatched on the different thread than the JavaScript runtime runs on.
    AsyncFunction("parseImage") { (imagePath: String, promise: Promise) in
      let logger = Logger()
      let latinOptions = TextRecognizerOptions()
      let latinTextRecognizer = TextRecognizer.textRecognizer(options: latinOptions)

      //      latinTextRecognizer.process(visionImage) { features, error in
      //        self.processResult(from: features, error: error)
      //      }

      Task {
        do {
          let img = try RNMLKitImage(imagePath: imagePath)
          let labels = try await latinTextRecognizer.process(img.visionImage)
        } catch {
          promise.reject(
            NSError(
              domain: ERROR_DOMAIN, code: 1,
              userInfo: [NSLocalizedDescriptionKey: "Error occurred: \(error)"])
          )
        }
      }
    }
  }
}
