//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "LookupMsg.h"

LookupMsg::LookupMsg(Peer* sender, double x) {
    this->sender = sender;
    this->x = x;
    this->hops = 0;
    this->setName("LookupMsg");

}

LookupMsg::~LookupMsg() {
    // TODO Auto-generated destructor stub
}

