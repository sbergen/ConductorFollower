#include "VisualizerImpl.h"

#include <boost/make_shared.hpp>

namespace cf {
namespace Visualizer {

boost::shared_ptr<Visualizer>
Visualizer::Create()
{
	return boost::make_shared<VisualizerImpl>();
}

void
VisualizerImpl::UpdateData(VisualizationData & data)
{

}

} // namespace Visualizer
} // namespace cf


