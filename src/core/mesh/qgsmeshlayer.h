/***************************************************************************
                         qgsmeshlayer.h
                         --------------
    begin                : April 2018
    copyright            : (C) 2018 by Peter Petrik
    email                : zilolv at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSMESHLAYER_H
#define QGSMESHLAYER_H

#include <memory>

#include "qgis_core.h"
#include "qgsmaplayer.h"
#include "qgsrendercontext.h"
#include "qgsmeshdataprovider.h"
#include "qgsmeshrenderersettings.h"

class QgsMapLayerRenderer;
class QgsSymbol;
class QgsTriangularMesh;
struct QgsMesh;

/**
 * \ingroup core
 *
 * Represents a mesh layer supporting display of data on structured or unstructured meshes
 *
 * The QgsMeshLayer is instantiated by specifying the name of a data provider,
 * such as mdal, and url defining the specific data set to connect to.
 * The vector layer constructor in turn instantiates a QgsMeshDataProvider subclass
 * corresponding to the provider type, and passes it the url. The data provider
 * connects to the data source.
 *
 * The QgsMeshLayer provides a common interface to the different data types. It does not
 * yet support editing transactions.
 *
 * The main data providers supported by QGIS are listed below.
 *
 * \section mesh_providers Mesh data providers
 *
 * \subsection mesh_memory Memory data providerType (mesh_memory)
 *
 * The memory data provider is used to construct in memory data, for example scratch
 * data. There is no inherent persistent storage of the data. The data source uri is constructed.
 * Data can be populated by setMesh(const QString &vertices, const QString &faces), where
 * vertices and faces is comma separated coordinates and connections for mesh.
 * E.g. to create mesh with one quad and one triangle
 *
 * \code
 *  QString uri(
 *      "1.0, 2.0 \n" \
 *      "2.0, 2.0 \n" \
 *      "3.0, 2.0 \n" \
 *      "2.0, 3.0 \n" \
 *      "1.0, 3.0 \n" \
 *      "---"
 *      "0, 1, 3, 4 \n" \
 *      "1, 2, 3 \n"
 *    );
 *    QgsMeshLayer *scratchLayer = new QgsMeshLayer(uri, "My Scratch layer", "memory_mesh");
 * \endcode
 *
 * \subsection mdal MDAL data provider (mdal)
 *
 * Accesses data using the MDAL drivers (https://github.com/lutraconsulting/MDAL). The url
 * is the MDAL connection string. QGIS must be built with MDAL support to allow this provider.

 * \code
 *     QString uri = "test/land.2dm";
 *     QgsMeshLayer *scratchLayer = new QgsMeshLayer(uri, "My Scratch Layer",  "mdal");
 * \endcode
 *
 * \note The API is considered EXPERIMENTAL and can be changed without a notice
 *
 * \since QGIS 3.2
 */
class CORE_EXPORT QgsMeshLayer : public QgsMapLayer
{
    Q_OBJECT
  public:

    /**
     * Setting options for loading mesh layers.
     */
    struct LayerOptions
    {
      int unused;  //! @todo remove me once there are actual members here (breaks SIP <4.19)
    };

    /**
     * Constructor - creates a mesh layer
     *
     * The QgsMeshLayer is constructed by instantiating a data provider.  The provider
     * interprets the supplied path (url) of the data source to connect to and access the
     * data.
     *
     * \param path  The path or url of the parameter.  Typically this encodes
     *               parameters used by the data provider as url query items.
     * \param baseName The name used to represent the layer in the legend
     * \param providerLib  The name of the data provider, e.g., "mesh_memory", "mdal"
     * \param options general mesh layer options
     */
    explicit QgsMeshLayer( const QString &path = QString(), const QString &baseName = QString(), const QString &providerLib = "mesh_memory",
                           const QgsMeshLayer::LayerOptions &options = QgsMeshLayer::LayerOptions() );
    ~QgsMeshLayer() override;

    //! QgsMeshLayer cannot be copied.
    QgsMeshLayer( const QgsMeshLayer &rhs ) = delete;
    //! QgsMeshLayer cannot be copied.
    QgsMeshLayer &operator=( QgsMeshLayer const &rhs ) = delete;

    QgsMeshDataProvider *dataProvider() override;
    const QgsMeshDataProvider *dataProvider() const override SIP_SKIP;
    QgsMeshLayer *clone() const override SIP_FACTORY;
    QgsRectangle extent() const override;
    QgsMapLayerRenderer *createMapRenderer( QgsRenderContext &rendererContext ) override SIP_FACTORY;
    bool readSymbology( const QDomNode &node, QString &errorMessage, QgsReadWriteContext &context ) override;
    bool writeSymbology( QDomNode &node, QDomDocument &doc, QString &errorMessage, const QgsReadWriteContext &context ) const override;
    QString encodedSource( const QString &source, const QgsReadWriteContext &context ) const override;
    QString decodedSource( const QString &source, const QString &provider, const QgsReadWriteContext &context ) const override;
    bool readXml( const QDomNode &layer_node, QgsReadWriteContext &context ) override;
    bool writeXml( QDomNode &layer_node, QDomDocument &doc, const QgsReadWriteContext &context ) const override;

    //! Returns the provider type for this layer
    QString providerType() const;

    //! Returns native mesh (nullptr before rendering)
    QgsMesh *nativeMesh() SIP_SKIP;

    //! Returns triangular mesh (nullptr before rendering)
    QgsTriangularMesh *triangularMesh() SIP_SKIP;

    //! Returns renderer settings
    QgsMeshRendererMeshSettings rendererNativeMeshSettings() const;

    //! Sets new renderer settings, triggers repaint
    void setRendererNativeMeshSettings( const QgsMeshRendererMeshSettings &settings );

    //! Returns renderer settings
    QgsMeshRendererMeshSettings rendererTriangularMeshSettings() const;

    //! Sets new renderer settings, triggers repaint
    void setRendererTriangularMeshSettings( const QgsMeshRendererMeshSettings &settings );

    //! Returns renderer settings
    QgsMeshRendererScalarSettings rendererScalarSettings() const;

    //! Sets new renderer settings, triggers repaint
    void setRendererScalarSettings( const QgsMeshRendererScalarSettings &settings );

    //! Returns renderer settings
    QgsMeshRendererVectorSettings rendererVectorSettings() const;

    //! Sets new renderer settings, triggers repaint
    void setRendererVectorSettings( const QgsMeshRendererVectorSettings &settings );

    /**
     * Sets active scalar dataset for rendering
     *
     * Triggers repaint
     */
    void setActiveScalarDataset( QgsMeshDatasetIndex index = QgsMeshDatasetIndex() );
    //! Returns active scalar dataset
    QgsMeshDatasetIndex activeScalarDataset() const { return mActiveScalarDataset; }

    /**
     * Sets active vector dataset for rendering.
     *
     * If dataset is not vector based, do nothing. Triggers repaint
     */
    void setActiveVectorDataset( QgsMeshDatasetIndex index = QgsMeshDatasetIndex() );
    //! Returns active vector dataset
    QgsMeshDatasetIndex activeVectorDataset() const { return mActiveVectorDataset; }

  private: // Private methods

    /**
     * Returns true if the provider is in read-only mode
     */
    bool isReadOnly() const override {return true;}

    /**
     * Binds layer to a specific data provider
     * \param provider provider key string, must match a valid QgsMeshDataProvider key. E.g. "mesh_memory", etc.
     * \param options generic provider options
     */
    bool setDataProvider( QString const &provider, const QgsDataProvider::ProviderOptions &options );

#ifdef SIP_RUN
    QgsMeshLayer( const QgsMeshLayer &rhs );
#endif

  private:
    void fillNativeMesh();

  private:
    //! Pointer to data provider derived from the abastract base class QgsMeshDataProvider
    QgsMeshDataProvider *mDataProvider = nullptr;

    //! Data provider key
    QString mProviderKey;

    //! Pointer to native mesh structure, used as cache for rendering
    std::unique_ptr<QgsMesh> mNativeMesh;

    //! Pointer to derived mesh structure
    std::unique_ptr<QgsTriangularMesh> mTriangularMesh;

    QgsMeshRendererMeshSettings mRendererNativeMeshSettings;
    QgsMeshRendererMeshSettings mRendererTriangularMeshSettings;
    QgsMeshRendererScalarSettings mRendererScalarSettings;
    QgsMeshRendererVectorSettings mRendererVectorSettings;

    //! index of active scalar dataset
    QgsMeshDatasetIndex mActiveScalarDataset;

    //! index of active vector dataset
    QgsMeshDatasetIndex mActiveVectorDataset;
};

#endif //QGSMESHLAYER_H
