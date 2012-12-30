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

package symphony;

simple Peer {
    parameters:
        bool isMemberOfAStaticNetwork = default(false);
        int k = default(2); //Long distance links to maintain. 2k is the upper limit for the incoming links. See the paper.
        int attemptsUpperBound = default(5); 
        double id = default(uniform(0,1));
    gates:
        input shortLinkIn[2] @loose;
        output shortLinkOut[2] @loose;
        input longDistanceLinkIn[];
        output longDistanceLinkOut[];
}
