//
//  telemetry.hpp
//  Lighthouse Camera
//
//  Created by David Teller on 02/02/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef telemetry_h
#define telemetry_h


// ---- Telemetry categegories

/**
 * Anything related to computer vision.
 */
#define TELEMETRY_CATEGORY_VISION "vision"

// ---- Telemetry actions

/**
 * The action of identifying an object.
 */
#define TELEMETRY_ACTION_IDENTIFY_OBJECT "identify_object"
#define TELEMETRY_ACTION_RECORD_OBJECT "record_object"

// ---- Telemetry labels

/**
 * An operation has succeeded.
 *
 * No measure is expected for this operation.
 */
#define TELEMETRY_LABEL_SUCCESS "ok"

/**
 * An operation has failed and returned an error code.
 *
 * Expects as measure an error code.
 *
 * If your error codes can change, you should rather use strings for the errors.
 */
#define TELEMETRY_LABEL_ERROR_CODE "errcode"

/**
 * The duration of an operation, in ms.
 */
#define TELEMETRY_LABEL_DURATION_MS "duration_ms"


#endif /* telemetry_h */
