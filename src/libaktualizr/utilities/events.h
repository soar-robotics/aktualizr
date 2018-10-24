#ifndef EVENTS_H_
#define EVENTS_H_
/** \file */

#include <map>
#include <string>

#include <json/json.h>
#include <boost/signals2.hpp>

#include "campaign/campaign.h"
#include "uptane/tuf.h"
#include "utilities/results.h"
#include "utilities/types.h"

/**
 * Aktualizr status events.
 */
namespace event {

/**
 * Base class for all event objects.
 */
class BaseEvent {
 public:
  BaseEvent() = default;
  BaseEvent(std::string variant_in) : variant(std::move(variant_in)) {}
  virtual ~BaseEvent() = default;
  std::string variant;
};

/**
 * Device data has been sent to the server.
 */
class SendDeviceDataComplete : public BaseEvent {
 public:
  explicit SendDeviceDataComplete();
};

/**
 * A manifest has been sent to the server.
 */
class PutManifestComplete : public BaseEvent {
 public:
  explicit PutManifestComplete(bool success_in);
  bool success;
};

/**
 * An update is available for download from the server.
 */
class UpdateCheckComplete : public BaseEvent {
 public:
  explicit UpdateCheckComplete(UpdateCheckResult result_in);
  UpdateCheckResult result;
};

/**
 * A report for a download in progress.
 */
class DownloadProgressReport : public BaseEvent {
 public:
  DownloadProgressReport(Uptane::Target target_in, std::string description_in, unsigned int progress_in);
  Uptane::Target target;
  std::string description;
  unsigned int progress;
};

/**
 * A target has been downloaded.
 */
class DownloadTargetComplete : public BaseEvent {
 public:
  DownloadTargetComplete(Uptane::Target update_in, bool success_in);
  Uptane::Target update;
  bool success;
};

/**
 * All targets for an update have been downloaded.
 */
class AllDownloadsComplete : public BaseEvent {
 public:
  explicit AllDownloadsComplete(DownloadResult result_in);
  DownloadResult result;
};

/**
 * An ECU has begun installation of an update.
 */
class InstallStarted : public BaseEvent {
 public:
  explicit InstallStarted(Uptane::EcuSerial serial_in);
  Uptane::EcuSerial serial;
};

/**
 * An installation attempt on an ECU has completed.
 */
class InstallTargetComplete : public BaseEvent {
 public:
  InstallTargetComplete(Uptane::EcuSerial serial_in, bool success_in);
  Uptane::EcuSerial serial;
  bool success;
};

/**
 * All ECU installation attempts for an update have completed.
 */
class AllInstallsComplete : public BaseEvent {
 public:
  explicit AllInstallsComplete(InstallResult result_in);
  InstallResult result;
};

/**
 * The server has been queried for available campaigns.
 */
class CampaignCheckComplete : public BaseEvent {
 public:
  explicit CampaignCheckComplete(CampaignCheckResult result_in);
  CampaignCheckResult result;
};

/**
 * A campaign has been accepted.
 */
class CampaignAcceptComplete : public BaseEvent {
 public:
  explicit CampaignAcceptComplete();
};

using Channel = boost::signals2::signal<void(std::shared_ptr<event::BaseEvent>)>;

}  // namespace event

#endif  // EVENTS_H_
