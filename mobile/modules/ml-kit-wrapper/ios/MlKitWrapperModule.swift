import ExpoModulesCore
import Foundation
import Vision

let ERROR_DOMAIN: String = "red.infinite.react-native-mlkit.ImageLabelerErrorDomain"

public enum RNMLKitImageError: Error {
  case invalidImageURL(imagePath: String)
  case couldNotLoadImage(imagePath: String)
  case cgImageEmpty(imagePath: String)
}

struct Result: Record {
  @Field
  public var text = ""

  @Field
  public var minX = CGFloat()

  @Field
  public var maxX = CGFloat()

  @Field
  public var minY = CGFloat()

  @Field
  public var maxY = CGFloat()
}

public class RNMLKitImage {
  public var imageURL: URL
  public var uiImage: UIImage

  public init(imagePath: String) throws {
    guard let imageURL = URL(string: imagePath) else {
      throw RNMLKitImageError.invalidImageURL(imagePath: imagePath)
    }

    self.imageURL = imageURL

    guard let uiImage = UIImage(contentsOfFile: imageURL.path) else {
      throw RNMLKitImageError.invalidImageURL(imagePath: imagePath)
    }

    self.uiImage = uiImage

  }
}

public class MlKitWrapperModule: Module {
  // Each module class must implement the definition function. The definition consists of components
  // that describes the module's functionality and behavior.
  // See https://docs.expo.dev/modules/module-api for more details about available components.
  public func definition() -> ModuleDefinition {
    // Sets the name of the module that JavaScript code will use to refer to the module. Takes a string as an argument.
    // Can be inferred from module's class name, but it's recommended to set it explicitly for clarity.
    // The module will be accessible from `requireNativeModule('MlKitWrapper')` in JavaScript.
    Name("MlKitWrapper")

    AsyncFunction("parseImage") { (imagePath: String, promise: Promise) in
      Task {
        do {
          let img = try RNMLKitImage(imagePath: imagePath)
          if img.uiImage.cgImage == nil {
            throw RNMLKitImageError.cgImageEmpty(imagePath: imagePath)
          }

          let requestHandler = VNImageRequestHandler(cgImage: img.uiImage.cgImage!)
          let request = VNRecognizeTextRequest()
          request.recognitionLevel = .accurate

          try requestHandler.perform([request])

          if let detectedText = request.results as? [VNRecognizedTextObservation] {
              
//            let recognizedStrings: [Result] = detectedText.compactMap { observation in
//              // Return the string of the top VNRecognizedText instance.
//
//              guard let candidate = observation.topCandidates(1).first else { return nil }
//
//              // Find the bounding-box observation for the string range.
//              let stringRange = candidate.string.startIndex..<candidate.string.endIndex
//              let boxObservation = try? candidate.boundingBox(for: stringRange)
//
//              // Get the normalized CGRect value.
//              let boundingBox = boxObservation?.boundingBox ?? .zero
//
//              let result = Result()
//
//              result.text = candidate.string
//              result.minX = boundingBox?.minX
//              result.maxX = observation.maxX
//              result.maxY = observation.maxY
//              result.minY = observation.minY
//
//              return result
//            }

            // promise.resolve(recognizedStrings)
          } else {
            promise.reject(
              NSError(
                domain: ERROR_DOMAIN, code: 1,
                userInfo: [NSLocalizedDescriptionKey: "Parsing error"])
            )
            return
          }
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
