/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "DevsimLoader.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "Coordinate.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Contact.hh"
#include "Interface.hh"
#include "NodeSolution.hh"
#include "EdgeSubModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "ModelCreate.hh"
#include "OutputStream.hh"
#include "EquationFunctions.hh"
#include "dsAssert.hh"
#include "Interpreter.hh"
#include <sstream>

namespace dsMesh {

namespace {
#if 0
template <typename T> void DeletePointersFromVector(T &x)
{
    typename T::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete *it;
    }
}
#endif

template <typename T> void DeletePointersFromMap(T &x)
{
    typename T::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete it->second;
    }
}
}


DevsimLoader::DevsimLoader(const std::string &n) : Mesh(n)
{
    //// Arbitrary number
    coordinateList.reserve(1000);
}
;

DevsimLoader::~DevsimLoader() {
//    DeletePointers<MeshCoordinateList_t>(coordinateList);
      DeletePointersFromMap<MeshRegionList_t>(regionList);
      DeletePointersFromMap<MeshInterfaceList_t>(interfaceList);
      DeletePointersFromMap<MeshContactList_t>(contactList);
}

namespace {
void processNodes(const MeshRegion &mr, const Device::CoordinateList_t &clist, std::vector<const Node *> &nlist)
{
  const MeshRegion::NodeList_t &nl = mr.GetNodes();
  MeshRegion::NodeList_t::const_iterator nit = nl.begin();
  for (size_t ni = 0; nit != nl.end(); ++nit, ++ni)
  {
    const size_t index = nit->Index();
    dsAssert(index < clist.size(), "UNEXPECTED");
    Node *np = new Node(ni, clist[index]);
    nlist.push_back(np);
  }
}

size_t processEdges(const MeshRegion &mr, const std::vector<const Node *> &nlist, std::vector<const Edge *> &elist)
{
  MeshRegion::EdgeList_t el = mr.GetEdges();
  size_t edgeduplicate = 0;
  std::sort(el.begin(), el.end());
  MeshRegion::EdgeList_t::iterator elnewend = std::unique(el.begin(), el.end());
  if (elnewend != el.end())
  {
    edgeduplicate = el.size();
    el.erase(elnewend, el.end());
    edgeduplicate -= el.size();
  }

  MeshRegion::EdgeList_t::const_iterator eit = el.begin();
  for (size_t ei = 0; eit != el.end(); ++eit, ++ei)
  {
    const size_t index0 = eit->Index0();
    const size_t index1 = eit->Index1();
    dsAssert(index0 < nlist.size(), "UNEXPECTED");
    dsAssert(index1 < nlist.size(), "UNEXPECTED");
    Edge *ep = new Edge(ei, nlist[index0], nlist[index1]);
    elist.push_back(ep);
  }
  return edgeduplicate;
}

size_t processTriangles(const MeshRegion &mr, const std::vector<const Node *> &nlist, std::vector<const Triangle *> &triangle_list)
{
  MeshRegion::TriangleList_t tl = mr.GetTriangles();
  size_t triangleduplicate = 0;
  std::sort(tl.begin(), tl.end());
  MeshRegion::TriangleList_t::iterator tlnewend = std::unique(tl.begin(), tl.end());

  if (tlnewend != tl.end())
  {
    triangleduplicate = tl.size();
    tl.erase(tlnewend, tl.end());
    triangleduplicate -= tl.size();
  }

  MeshRegion::TriangleList_t::const_iterator tit = tl.begin();
  for (size_t ti = 0; tit != tl.end(); ++tit, ++ti)
  {
    const size_t index0 = tit->Index0();
    const size_t index1 = tit->Index1();
    const size_t index2 = tit->Index2();
    dsAssert(index0 < nlist.size(), "UNEXPECTED");
    dsAssert(index1 < nlist.size(), "UNEXPECTED");
    dsAssert(index2 < nlist.size(), "UNEXPECTED");
    Triangle *tp = new Triangle(ti, nlist[index0], nlist[index1], nlist[index2]);
    triangle_list.push_back(tp);
//          os << "Triangle: " << index0 << "\t" << index1 << "\t" << index2 <<  "\t" << nlist[index0] << "\t" << nlist[index1] << "\t" << nlist[index2] << "\n";
  }
  return triangleduplicate;
}

size_t processTetrahedra(const MeshRegion &mr, const std::vector<const Node *> &nlist, std::vector<const Tetrahedron *> &tetrahedron_list)
{
  size_t tetrahedronduplicate = 0;

  MeshRegion::TetrahedronList_t tl = mr.GetTetrahedra();
  std::sort(tl.begin(), tl.end());
  MeshRegion::TetrahedronList_t::iterator tlnewend = std::unique(tl.begin(), tl.end());

  if (tlnewend != tl.end())
  {
    tetrahedronduplicate = tl.size();
    tl.erase(tlnewend, tl.end());
    tetrahedronduplicate -= tl.size();
  }

  MeshRegion::TetrahedronList_t::const_iterator tit = tl.begin();
  for (size_t ti = 0; tit != tl.end(); ++tit, ++ti)
  {
      const size_t index0 = tit->Index0();
      const size_t index1 = tit->Index1();
      const size_t index2 = tit->Index2();
      const size_t index3 = tit->Index3();
      dsAssert(index0 < nlist.size(), "UNEXPECTED");
      dsAssert(index1 < nlist.size(), "UNEXPECTED");
      dsAssert(index2 < nlist.size(), "UNEXPECTED");
      dsAssert(index3 < nlist.size(), "UNEXPECTED");
      Tetrahedron *tp = new Tetrahedron(ti, nlist[index0], nlist[index1], nlist[index2], nlist[index3]);
      tetrahedron_list.push_back(tp);
//          os << "Triangle: " << index0 << "\t" << index1 << "\t" << index2 <<  "\t" << nlist[index0] << "\t" << nlist[index1] << "\t" << nlist[index2] << "\n";
  }
  return tetrahedronduplicate;
}

void createRegion(const MeshRegion &mr, Device *dp, std::vector<const Node *> &nlist,
  std::vector<const Edge *> &elist, std::vector<const Triangle *> &triangle_list,
  std::vector<const Tetrahedron *> &tetrahedron_list)
{
  const std::string &rname = mr.GetName();
  const std::string &material = mr.GetMaterial();

  const Device::CoordinateList_t &clist = dp->GetCoordinateList();

  nlist.reserve(clist.size());

  size_t dimension = 1;
  if (mr.HasTetrahedra())
  {
    dimension = 3;
  }
  else if (mr.HasTriangles())
  {
    dimension = 2;
  }
  else
  {
    dimension = 1;
  }

  Region *rp = new Region(rname, material, dimension, dp);
  dp->AddRegion(rp);

  Region &region = *rp;

  /////
  ///// Nodes
  /////
  processNodes(mr, clist, nlist);

  region.AddNodeList(nlist);

  {
    std::ostringstream os; 
    os << "Adding " << nlist.size() << " nodes" << "\n";
    OutputStream::WriteOut(OutputStream::INFO, os.str());
  }

  /////
  ///// Edges
  /////
  size_t edgeduplicate = processEdges(mr, nlist, elist);

  region.AddEdgeList(elist);

  {
    std::ostringstream os; 
    os << "Adding " << elist.size() << " edges with " << edgeduplicate << " duplicates removed\n";
    OutputStream::WriteOut(OutputStream::INFO, os.str());
  }

  ////
  //// Triangles
  ////
  size_t triangleduplicate = processTriangles(mr, nlist, triangle_list);

  region.AddTriangleList(triangle_list);

  {
    std::ostringstream os; 
    os << "Adding " << triangle_list.size() << " triangles " << " with " << triangleduplicate << " duplicate removed\n";
    OutputStream::WriteOut(OutputStream::ERROR, os.str());
  }

  ////
  //// Tetrahedra
  ////
  if (dimension == 3)
  {
    size_t tetrahedronduplicate = processTetrahedra(mr, nlist, tetrahedron_list);
    region.AddTetrahedronList(tetrahedron_list);
    {
      std::ostringstream os; 
      os << "Adding " << tetrahedron_list.size() << " tetrahedra " << " with " << tetrahedronduplicate << " duplicate removed\n";
      OutputStream::WriteOut(OutputStream::ERROR, os.str());
    }
  }

  //// Beware this changes the internal sort order
  //// So we need to keep our external lists until the end
  region.FinalizeMesh();
  CreateDefaultModels(rp);
}

}

bool DevsimLoader::RunCommand(const std::string &commandString, const std::vector<std::pair<std::string, ObjectHolder> > &commandArgs, std::string &resultString)
{
  Interpreter MyInterp;
  bool ret = MyInterp.RunInternalCommand(commandString, commandArgs);

  if (ret)
  {
    resultString = MyInterp.GetResult().GetString();
  }
  else
  {
    resultString = MyInterp.GetErrorString();
  }

  return ret;
}

bool DevsimLoader::Instantiate_(const std::string &deviceName, std::string &errorString)
{
  bool ret = true;
  GlobalData &gdata = GlobalData::GetInstance();

  if (gdata.GetDevice(deviceName))
  {
    std::ostringstream os; 
    os << deviceName << " already exists\n";
    errorString += os.str();
    ret = false;
    return ret;
  }

  size_t ncoords = coordinateList.size();

  MeshRegionList_t::iterator it = regionList.begin();

  size_t dimension = 1;
  if ((*(it->second)).HasTetrahedra())
  {
    dimension = 3;
  }
  else if ((*(it->second)).HasTriangles())
  {
    dimension = 2;
  }

  DevicePtr dp = new Device(deviceName, dimension);
  gdata.AddDevice(dp);

  Device::CoordinateList_t clist;
  clist.reserve(ncoords);
  for (MeshCoordinateList_t::iterator it = coordinateList.begin(); it != coordinateList.end(); ++it)
  {
    clist.push_back(new Coordinate(it->GetX(), it->GetY(), it->GetZ()));
  }

  dp->AddCoordinateList(clist);

  //// Use for contacts and interfaces
  std::map<std::string, std::vector<const Node *> > node_map;
  std::map<std::string, std::vector<const Edge *> > edge_map;
  std::map<std::string, std::vector<const Triangle *> > triangle_map;
  std::map<std::string, std::vector<const Tetrahedron *> > tetrahedron_map;

  {
    std::vector<const Node *> nlist;
    std::vector<const Edge *> elist;
    std::vector<const Triangle *>    triangle_list;
    std::vector<const Tetrahedron *> tetrahedron_list;

    nlist.reserve(clist.size());

    for (MeshRegionList_t::iterator it = regionList.begin(); it != regionList.end(); ++it)
    {
      const std::string  &rname = it->first;
      const MeshRegion   &mr = *(it->second);

      createRegion(mr, dp, nlist, elist, triangle_list, tetrahedron_list);
      node_map[rname] = nlist;
      edge_map[rname] = elist;
      triangle_map[rname] = triangle_list;
      tetrahedron_map[rname] = tetrahedron_list;

      nlist.clear();
      elist.clear();
      triangle_list.clear();
      tetrahedron_list.clear();
    }
  }

  {
    ConstNodeList_t cnl;
    for (MeshContactList_t::iterator it = contactList.begin(); it != contactList.end(); ++it)
    {
      const std::string &cname = it->first;
      MeshContact &cnt = *(it->second);
      const std::string &rname = cnt.GetRegion();

      std::string errorString;

      const RegionPtr rp = dp->GetRegion(rname);

      if (!rp)
      {
        ret = false;
        std::ostringstream os; 
        os << "Region " << rname << " does not exist for contact " << cname << "\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        break;
      }
      dsAssert(node_map.count(rname) != 0, "UNEXPECTED");

      ConstNodeList_t &nl = node_map[rname];

      const MeshContact::NodeList_t &mcl = cnt.GetNodes();
      for (MeshContact::NodeList_t::const_iterator cit = mcl.begin(); cit != mcl.end(); ++cit)
      {
        size_t index = cit->Index();
        if (index < nl.size())
        {
          cnl.push_back(nl[index]);
        }
        else
        {
          ret = false;
          std::ostringstream os; 
          os << "Node index " << index << " for contact " << cname << " violates bounds for region " << rname << "\n";
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
        }
      }

      if (!ret)
      {
          break;
      }
      else
      {
        dp->AddContact(new Contact(cname, rp, cnl));
        std::ostringstream os; 
        os << "Contact " << cname << " in region " << rname << " with " << cnl.size() << " nodes" << "\n";
        OutputStream::WriteOut(OutputStream::INFO, os.str());
      }
      cnl.clear();
    }
    if (!ret)
    {
      errorString += "Check log for errors.\n";
      return ret;
    }
  }

  {
    ConstNodeList_t cn0;
    ConstNodeList_t cn1;
    for (MeshInterfaceList_t::iterator it = interfaceList.begin(); it != interfaceList.end(); ++it)
    {
      const std::string &iname = it->first;
      MeshInterface &mint = *(it->second);

      const std::string &rname0 = mint.GetRegion0();
      const std::string &rname1 = mint.GetRegion1();

      const RegionPtr rp0 = dp->GetRegion(rname0);
      if (!rp0)
      {
        ret = false;
        std::ostringstream os; 
        os << "Region " << rname0 << " does not exist for interface " << iname << "\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        break;
      }
      dsAssert(node_map.count(rname0) != 0, "UNEXPECTED");
      const RegionPtr rp1 = dp->GetRegion(rname1);
      if (!rp1)
      {
        ret = false;
        std::ostringstream os; 
        os << "Region " << rname1 << " does not exist for interface " << iname << "\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
        break;
      }
      dsAssert(node_map.count(rname1) != 0, "UNEXPECTED");

      const ConstNodeList_t &nl0 = node_map[rname0];
      const ConstNodeList_t &nl1 = node_map[rname1];

      const MeshInterface::NodePairList_t &mnp = mint.GetNodePairs();
      {
        std::ostringstream os; 
        os << "Interface " << iname << " " << rname0 << " " << rname1 << " has " << mnp.size() << " node pairs" << "\n";
        OutputStream::WriteOut(OutputStream::INFO, os.str());
      }

      for (MeshInterface::NodePairList_t::const_iterator iit = mnp.begin(); iit != mnp.end(); ++iit)
      {
        size_t index0 = iit->Index0();
        size_t index1 = iit->Index1();
        if (index0 >= nl0.size())
        {
          ret = false;
          std::ostringstream os; 
          os << "Node index " << index0 << " for interface " << iname << " violates bounds for region " << rname0 << "\n";
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
        }
        else if (index1 >= nl1.size())
        {
          ret = false;
          std::ostringstream os; 
          os << "Node index " << index1 << " for interface " << iname << " violates bounds for region " << rname1 << "\n";
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
        }
        else
        {
          cn0.push_back(nl0[index0]);
          cn1.push_back(nl1[index1]);
        }
      }

      if (!ret)
      {
        break;
      }
      else
      {
        std::ostringstream os; 
        os << "Adding interface " << iname << " with " << cn0.size() << ", " << cn1.size() << " nodes" << "\n";
        OutputStream::WriteOut(OutputStream::INFO, os.str());
        dp->AddInterface(new Interface(iname, rp0, rp1, cn0, cn1));
      }
      cn0.clear();
      cn1.clear();
    }
    if (!ret)
    {
      errorString += "Check log for errors.\n";
      return ret;
    }
  }


  for (MeshRegionList_t::iterator it = regionList.begin(); it != regionList.end(); ++it)
  {
    const std::string  &rname = it->first;
    const MeshRegion   &mr = *(it->second);
    const MeshRegion::SolutionList_t &sl = mr.GetSolutionList();

    const RegionPtr rp = dp->GetRegion(rname);

    const ConstNodeList        &node_list        = node_map[rname];
    const ConstEdgeList        &edge_list        = edge_map[rname];
//    const ConstTriangleList    &triangle_list    = triangle_map[rname];
//    const ConstTetrahedronList &tetrahedron_list = tetrahedron_map[rname];

    if (!rp)
    {
      ret = false;
      std::ostringstream os; 
      os << "Region " << rname << " does not exis\n";
      OutputStream::WriteOut(OutputStream::ERROR, os.str());
      break;
    }

    for (MeshRegion::SolutionList_t::const_iterator sit = sl.begin(); sit != sl.end(); ++sit)
    {
      const std::string &sname = sit->first;

      const Solution    &sol   = *(sit->second);
      Solution::ModelType  model_type = sol.GetModelType();
      Solution::DataType   data_type  = sol.GetDataType();
//      Solution::values_t vals  = sol.GetValues();

      const bool alreadyExists =
        (
          ((model_type == Solution::NODE) && rp->GetNodeModel(sname)) ||
          ((model_type == Solution::EDGE) && rp->GetEdgeModel(sname)) ||
          ((model_type == Solution::TRIANGLEEDGE) && rp->GetTriangleEdgeModel(sname)) ||
          ((model_type == Solution::TETRAHEDRONEDGE) && rp->GetTetrahedronEdgeModel(sname))
        );

      if (alreadyExists)
      {
        if ((data_type !=  Solution::BUILTIN) && (data_type != Solution::DATAPARENT))
        {
          std::ostringstream os; 
          os << "Not replacing built in " << Solution::ModelTypeString[model_type] <<  " model " << sname << "\n";      
          OutputStream::WriteOut(OutputStream::INFO, os.str());
        }
      }
      else if (data_type == Solution::BUILTIN)
      {
        std::ostringstream os; 
        os << "Built in model missing " << sname << "\n";       
        OutputStream::WriteOut(OutputStream::INFO, os.str());
      }
      else
      {
        //// Here we instantiate the data
        //// In order to ensure that models aren't confused for parameters, COMMAND type will be created as an empty solution
        //// on the next pass, we will create the command types
        if (model_type == Solution::NODE)
        {
          //// This creates a uniform model with value 0.0
          NodeModelPtr nodesol = NodeSolution::CreateNodeSolution(sname, rp);
          if (data_type == Solution::UNIFORM)
          {
            nodesol->SetValues(sol.GetUniformValue());
          }
          else if (data_type == Solution::DATA)
          {
            Solution::values_t vals  = sol.GetValues();
            if (vals.size() != rp->GetNumberNodes())
            {
              ret = false;
              std::ostringstream os; 
              os << "Node solution " << sname << " has a different number of values (" << vals.size() << ") then specified for the region " << rname << " (" << rp->GetNumberNodes() << ")\n";
              OutputStream::WriteOut(OutputStream::FATAL, os.str());
            }
            else
            {
              NodeScalarList nsl(vals.size());
              for (size_t i = 0; i < vals.size(); ++i)
              {
                  nsl[node_list[i]->GetIndex()] = vals[i];
              }
              nodesol->SetValues(nsl);
            }
          }
        }
        else if (model_type == Solution::EDGE)
        {
          //// This creates a uniform model with value 0.0
          //// Do we need to get the display type somehow in our data format??
          EdgeModel *edgesol = new EdgeSubModel(sname, rp, EdgeModel::SCALAR);
          if (data_type == Solution::UNIFORM)
          {
            edgesol->SetValues(sol.GetUniformValue());
          }
          else if (data_type == Solution::DATA)
          {
            Solution::values_t vals  = sol.GetValues();
            if (vals.size() != rp->GetNumberEdges())
            {
              ret = false;
              std::ostringstream os; 
              os << "Edge data " << sname << " has a different number of values (" << vals.size() << ") then specified for the region " << rname << " (" << rp->GetNumberEdges() << ")\n";
              OutputStream::WriteOut(OutputStream::FATAL, os.str());
            }
            else
            {
              EdgeScalarList esl(vals.size());
              for (size_t i = 0; i < vals.size(); ++i)
              {
                esl[edge_list[i]->GetIndex()] = vals[i];
              }
              edgesol->SetValues(esl);
            }
          }
        }
        else if (model_type == Solution::TRIANGLEEDGE)
        {
          //// This creates a uniform model with value 0.0
          //// Do we need to get the display type somehow in our data format??
          TriangleEdgeModel *triangleedgesol = new TriangleEdgeSubModel(sname, rp, TriangleEdgeModel::SCALAR);
          if (data_type == Solution::UNIFORM)
          {
            triangleedgesol->SetValues(sol.GetUniformValue());
          }
          else if (data_type == Solution::DATA)
          {
            ret = false;
            std::ostringstream os; 
            os << "Triangle edge data is not supported " << sname << "\n";
            OutputStream::WriteOut(OutputStream::FATAL, os.str());
          }
        }
        else if (model_type == Solution::TETRAHEDRONEDGE)
        {
          //// This creates a uniform model with value 0.0
          //// Do we need to get the display type somehow in our data format??
          TetrahedronEdgeModel *tetrahedronedgesol = new TetrahedronEdgeSubModel(sname, rp, TetrahedronEdgeModel::SCALAR);
          if (data_type == Solution::UNIFORM)
          {
            tetrahedronedgesol->SetValues(sol.GetUniformValue());
          }
          else if (data_type == Solution::DATA)
          {
            ret = false;
            std::ostringstream os; 
            os << "Tetrahedron edge data is not supported " << sname << "\n";
            OutputStream::WriteOut(OutputStream::FATAL, os.str());
          }
        }
        ///// TODO: Note that fully supporting data on triangle edge and tetrahedron edge would require knowing the edge to element mapping
///////TODO: TRIANGLEEDGE (error out on data or non-uniform)
///////TODO: TETRAHEDRONEDGE (error out on data or non-uniform)
      }
    }
    for (MeshRegion::SolutionList_t::const_iterator sit = sl.begin(); sit != sl.end(); ++sit)
    {
//      const std::string &sname = sit->first;
      const Solution    &sol   = *(sit->second);
//      Solution::ModelType  model_type = sol.GetModelType();
      Solution::DataType   data_type  = sol.GetDataType();

      if (data_type == Solution::COMMAND)
      {
        std::ostringstream os;
        //// TODO: do command here!
//        os << "Execute: " << sol.GetCommandName() << "\n";
        {
          std::string resultString;
          ret = RunCommand(sol.GetCommandName(), sol.GetCommandArgs(), resultString);
          if (!ret)
          {
            errorString += "While executing: " + sol.GetCommandName() + "\n" + resultString + "\n";
            OutputStream::WriteOut(OutputStream::FATAL, errorString);
          }
//          os << "with result " << resultString << "\n";
        }
//        OutputStream::WriteOut(OutputStream::FATAL, os.str());
      }
    }

    const MeshRegion::EquationList_t &el = mr.GetEquationList();
    for (MeshRegion::EquationList_t::const_iterator eit = el.begin(); eit != el.end(); ++eit)
    {
      const Equation &equation   = *(eit->second);
      {
        std::string resultString;
        ret = RunCommand(equation.GetCommandName(), equation.GetCommandArgs(), resultString);
        if (!ret)
        {
          errorString += "While executing: " + equation.GetCommandName() + "\n" + resultString + "\n";
          OutputStream::WriteOut(OutputStream::FATAL, errorString);
        }
      }
    }
  }

//// Two passes, first to prevent model dependency problems
  for (MeshInterfaceList_t::iterator it = interfaceList.begin(); it != interfaceList.end(); ++it)
  {
    const std::string &iname = it->first;
    MeshInterface &mint = *(it->second);
    const InterfacePtr ip = dp->GetInterface(iname);

    const MeshInterface::SolutionList_t &sl = mint.GetSolutionList();
    for (MeshInterface::SolutionList_t::const_iterator sit = sl.begin(); sit != sl.end(); ++sit)
    {
      const std::string &sname = sit->first;
//      const Solution    &sol   = *(sit->second);
//      Solution::ModelType  model_type = sol.GetModelType();
//      Solution::DataType   data_type  = sol.GetDataType();
      //// TODO: need to assert it was successful
      dsHelper::ret_pair rval = dsHelper::CreateInterfaceNodeExprModel(sname, "0;", ip);
      if (!rval.first)
      {
        std::ostringstream os;
        //// TODO: do command here!
        os << "Unable to create interface model: " << sname << "\n";
        errorString += os.str();
        OutputStream::WriteOut(OutputStream::FATAL, os.str());
      }
    }
  }

  for (MeshInterfaceList_t::iterator it = interfaceList.begin(); it != interfaceList.end(); ++it)
  {
    MeshInterface &mint = *(it->second);
//    const InterfacePtr ip = dp->GetInterface(iname);

    const MeshInterface::SolutionList_t &sl = mint.GetSolutionList();

    for (MeshRegion::SolutionList_t::const_iterator sit = sl.begin(); sit != sl.end(); ++sit)
    {
//      const std::string &sname = sit->first;
      const Solution    &sol   = *(sit->second);
//      Solution::ModelType  model_type = sol.GetModelType();
      Solution::DataType   data_type  = sol.GetDataType();

      //// TODO: need to assert it was successful
      if (data_type == Solution::COMMAND)
      {
/*
        std::ostringstream os;
        //// TODO: do command here!
        os << "Need to execute: " << sol.GetCommandName() << "\n";
        OutputStream::WriteOut(OutputStream::INFO, os.str());
*/

        {
          std::string resultString;
          ret = RunCommand(sol.GetCommandName(), sol.GetCommandArgs(), resultString);
          if (!ret)
          {
            errorString += "While executing: " + sol.GetCommandName() + "\n" + resultString + "\n";
            OutputStream::WriteOut(OutputStream::FATAL, errorString);
          }
        }
      }
    }

    const MeshInterface::EquationList_t &el = mint.GetEquationList();
    for (MeshInterface::EquationList_t::const_iterator eit = el.begin(); eit != el.end(); ++eit)
    {
      const Equation &equation   = *(eit->second);
      {
        std::string resultString;
        ret = RunCommand(equation.GetCommandName(), equation.GetCommandArgs(), resultString);
        if (!ret)
        {
          errorString += "While executing: " + equation.GetCommandName() + "\n" + resultString + "\n";
          OutputStream::WriteOut(OutputStream::FATAL, errorString);
        }
      }
    }
  }

  for (MeshContactList_t::iterator it = contactList.begin(); it != contactList.end(); ++it)
  {
    MeshContact &cnt = *(it->second);
    const MeshContact::EquationList_t &el = cnt.GetEquationList();
    for (MeshContact::EquationList_t::const_iterator eit = el.begin(); eit != el.end(); ++eit)
    {
      const Equation &equation   = *(eit->second);
      {
        std::string resultString;
        ret = RunCommand(equation.GetCommandName(), equation.GetCommandArgs(), resultString);
        if (!ret)
        {
          errorString += "While executing: " + equation.GetCommandName() + "\n" + resultString + "\n";
          OutputStream::WriteOut(OutputStream::FATAL, errorString);
        }
      }
    }
  }

  if (!errorString.empty())
  {
    errorString += "Check log for errors.\n";
  }
  return ret;
}

///// This function should mostly just do error checking
bool DevsimLoader::Finalize_(std::string &errorString)
{
    bool ret = true;

    //// TODO: Finalize_

    /// ensure node, edge, and triangle indexes don't violate bounds of coordinate space

    /// ensure all coordinates utilized


    /// Make sure node intersections have interface
    /// No coordinates collinear


    ///// All regions the same dimension
  {
    size_t ncoords = coordinateList.size();
    size_t nregions = regionList.size();
    size_t ncontacts = contactList.size();
//      size_t ninterfaces = interfaceList.size();

    if (!ncoords)
    {
      ret = false;
      std::ostringstream os; 
      os << "There are no coordinates in the mesh " << this->GetName() << "\n";
      OutputStream::WriteOut(OutputStream::ERROR, os.str());
    }

    if (!nregions)
    {
      ret = false;
      std::ostringstream os; 
      os << "There are no regions on the mesh " << this->GetName() << "\n";
      OutputStream::WriteOut(OutputStream::ERROR, os.str());
    }

    if (ncontacts < 2)
    {
      ret = false;
      std::ostringstream os; 
      os << "There needs to be at least 2 contacts on the device and there are only " << ncontacts << "\n";
      OutputStream::WriteOut(OutputStream::ERROR, os.str());
    }


    size_t dimension = 0;

    MeshRegionList_t::iterator it = regionList.begin();

    if (it != regionList.end())
    {
      dimension = ((*(it->second)).HasTriangles()) ? 2 : 1;
    }

    for ( ; it != regionList.end(); ++it)
    {

      size_t nnodes = 0;
      //size_t nedges = 0;

      std::string  rname   = it->first;
      MeshRegion &region = *(it->second);

      size_t ldim = (region.HasTriangles()) ? 2 : 1;
      if (ldim != dimension)
      {
        ret = false;
        std::ostringstream os; 
        os << rname << " has dimension " << ldim << " while another region has dimension " << dimension << "\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
      }

      if (!region.HasNodes())
      {
        ret = false;
        std::ostringstream os; 
        os << rname << " has no nodes\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
      }
      else
      {
        const MeshRegion::NodeList_t &nl = region.GetNodes();
        nnodes = nl.size();
        if (nl.size() > ncoords)
        {
          ret = false;
          std::ostringstream os; 
          os << rname << " has more nodes than there are coordinates\n"; 
          OutputStream::WriteOut(OutputStream::ERROR, os.str());
        }
        else
        {
          for (MeshRegion::NodeList_t::const_iterator nit = nl.begin(); nit != nl.end(); ++nit)
          {
            if (nit->Index() >= ncoords)
            {
              ret = false;
              std::ostringstream os; 
              os << "Node index " << nit->Index() << " in region " << rname << "exceeds the max coordinate index, " << (ncoords - 1) << "\n";
              OutputStream::WriteOut(OutputStream::ERROR, os.str());
            }
          }
        }
      }

      if (!region.HasEdges())
      {
        ret = false;
        std::ostringstream os; 
        os << rname << " has no edges\n";
        OutputStream::WriteOut(OutputStream::ERROR, os.str());
      }
      else
      {
        const MeshRegion::EdgeList_t &nl = region.GetEdges();
        for (MeshRegion::EdgeList_t::const_iterator nit = nl.begin(); nit != nl.end(); ++nit)
        {
          if (nit->Index0() >= nnodes)
          {
            ret = false;
            std::ostringstream os; 
            os << "Node index " << nit->Index0() << " in region " << rname << " for edges exceeds the max node index, " << (nnodes - 1) << "\n";
            OutputStream::WriteOut(OutputStream::ERROR, os.str());
          }
          if (nit->Index1() >= nnodes)
          {
            ret = false;
            std::ostringstream os; 
            os << "Node index " << nit->Index1() << " in region " << rname << " for edges exceeds the max node index, " << (nnodes - 1) << "\n";
            OutputStream::WriteOut(OutputStream::ERROR, os.str());
          }
        }
      }

      if (region.HasTriangles())
      {
        const MeshRegion::TriangleList_t &nl = region.GetTriangles();
        for (MeshRegion::TriangleList_t::const_iterator nit = nl.begin(); nit != nl.end(); ++nit)
        {

          if (nit->Index0() >= nnodes)
          {
            ret = false;
            std::ostringstream os; 
            os << "Node index " << nit->Index0() << " in region " << rname << " for triangles exceeds the max node index, " << (nnodes - 1) << "\n";
            OutputStream::WriteOut(OutputStream::ERROR, os.str());
          }
          if (nit->Index1() >= nnodes)
          {
            ret = false;
            std::ostringstream os; 
            os << "Node index " << nit->Index1() << " in region " << rname << " for triangles exceeds the max node index, " << (nnodes - 1) << "\n";
            OutputStream::WriteOut(OutputStream::ERROR, os.str());
          }
          if (nit->Index2() >= nnodes)
          {
            ret = false;
            std::ostringstream os; 
            os << "Node index " << nit->Index2() << " in region " << rname << " for triangles exceeds the max node index, " << (nnodes - 1) << "\n";
            OutputStream::WriteOut(OutputStream::ERROR, os.str());
          }
        }
      }
    }
  }

  errorString += "Check log for errors.\n";
  if (ret)
  {
      this->SetFinalized();
  }
  return ret;
}

}
